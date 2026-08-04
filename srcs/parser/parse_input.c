/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_input.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nribakov <nribakov@student.42vienna.com    +#+  +:+       #+#        */
/*                                                +#+#+#+#+#+   #+#           */
/*   Created: 2026/06/10 17:14:19 by sancuta           #+#    #+#             */
/*   Updated: 2026/08/04 00:04:10 by nribakov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool	get_next_token(t_ctx *c, t_parser_state *parse, t_lexer_state *lex)
{
	while (true)
	{
		if ((parse->flags & PARSE_HERE_BODY)
			&& !handle_here_body(c, parse, lex))
			return (false);
		if (parse->flags & PARSE_HAS_SAVED_TOKENS)
		{
			if (handle_saved_tokens(c, parse))
				return (true);
		}
		if (lex->flags & LEX_AT_EOI)
		{
			ft_memset(lex, 0, sizeof(t_lexer_state));
			return (false);
		}
		if (lex_token(c, lex))
		{
			++parse->token_idx;
			return (true);
		}
	}
}

static void	report_parse_error(t_ctx *c, t_parser_state *parse)
{
	t_token	*token;
	char	*body;

	parse->flags |= PARSE_ERROR;
	token = get_ptr_from_idx(&c->arena[AT_TOKENS], parse->token_idx);
	body = get_ptr_from_offset(&c->arena[AT_STRING], token->offset);
	if (parse->flags & PARSE_LOOKAHEAD_EOF)
		printf("minishell: syntax error near unexpected token 'end of file'\n");
	else if (body[0] == '\n')
		printf("minishell: syntax error near unexpected token 'newline'\n");
	else
		printf("minishell: syntax error near unexpected token '%s'\n", body);
}

static bool	get_lookahead(t_ctx *c, t_parser_state *parse, t_lexer_state *lex)
{
	if (!get_next_token(c, parse, lex))
	{
		if (parse->flags & PARSE_SAVE_TOKENS)
		{
#ifdef DEBUG
			if (c->states & DBG_PARSER)
				fprintf(stderr, "--- parse --- saved tokens exhausted -> HERE_BODY mode\n");
#endif
			parse->flags &= ~PARSE_SAVE_TOKENS;
			parse->flags |= PARSE_HERE_BODY;
		}
		return (false);
	}
	return (true);
}

static bool	handle_here_doc(t_ctx *c, t_parser_state *parse)
{
	t_token	*cur;

	cur = get_ptr_from_idx(&c->arena[AT_TOKENS], parse->token_idx);
	if (c->arena[AT_STRING].buf[cur->offset] == '\n')
	{
#ifdef DEBUG
		if (c->states & DBG_PARSER)
			fprintf(stderr, "--- parse --- newline seen -> HERE_BODY mode\n");
#endif
		parse->flags &= ~PARSE_SAVE_TOKENS;
		parse->flags |= PARSE_HERE_BODY;
	}
	return (true);
}

#ifdef DEBUG
static void	debug_print_lookahead(t_ctx *c, t_parser_state *parse)
{
	fprintf(stderr, "\n--- lookahead ---\n");
	print_token(stderr, c, get_ptr_from_idx(&c->arena[AT_TOKENS], parse->token_idx));
	if (c->arenas & DBG_ARENA_STRING)
		print_arena(&c->arena[AT_STRING]);
	if (c->arenas & DBG_ARENA_TOKENS)
		print_arena(&c->arena[AT_TOKENS]);
}

static void	debug_parse_header(t_parser_state *parse)
{
	uint32_t	bit;
	uint8_t		flags;

	fprintf(stderr, "\n--- parse ---  token_idx = %lu  flags =", parse->token_idx);
	flags = parse->flags;
	bit = 1;
	while (bit && !(flags & bit))
		bit <<= 1;
	if (!bit)
		fprintf(stderr, " (none)");
	while (bit)
	{
		if (flags & bit)
		{
			fprintf(stderr, " %s", get_parse_flag_name(bit));
			flags ^= bit;
		}
		bit <<= 1;
	}
	fprintf(stderr, "\n");
}
#endif

static bool	parse_advance(t_ctx *c, t_parser_state *parse, bool *have_lookahead, t_lalr_action action)
{
	if (action == LALR_REDUCE)
		return (true);
	if (action == LALR_SHIFT)
	{
		*have_lookahead = false;
		return (true);
	}
	if (action == LALR_ACCEPT)
	{
		parse->flags |= PARSE_DONE;
		return (false);
	}
	report_parse_error(c, parse);
	*have_lookahead = false;
	return (false);
}

static bool	run_parse_iteration(t_ctx *c, t_parser_state *parse, t_lexer_state *lex, bool *have_lookahead)
{
	t_lalr_action	action;

#ifdef DEBUG
	debug_parse_header(parse);
#endif
	if (!*have_lookahead && !get_lookahead(c, parse, lex))
		return (false);
	*have_lookahead = true;
	if (parse->flags & PARSE_SAVE_TOKENS)
	{
		*have_lookahead = false;
		return (handle_here_doc(c, parse));
	}
#ifdef DEBUG
	debug_print_lookahead(c, parse);
#endif
	action = shift_reduce(c, parse, lex);
#ifdef DEBUG
	if (c->states & DBG_PARSER)
	{
		print_stack(c, parse);
		if (action == LALR_REDUCE || action == LALR_ACCEPT)
			print_nodes(stderr, c);
	}
#endif
	return (parse_advance(c, parse, have_lookahead, action));
}

static void	final_pass(t_ctx *c, t_parser_state *parse, t_lexer_state *lex)
{
	t_lalr_action	action;

	while (true)
	{
		action = shift_reduce(c, parse, lex);
		if (action == LALR_ACCEPT)
		{
			parse->flags |= PARSE_DONE;
			return ;
		}
		if (action == LALR_ERROR)
		{
			report_parse_error(c, parse);
			return ;
		}
	}
}

t_parser_state	parse_input(t_ctx *c)
{
	t_parser_state	parse;
	t_lexer_state	lex;
	bool			have_lookahead;

	ft_memset(&lex, 0, sizeof(t_lexer_state));
	ft_memset(&parse, 0, sizeof(t_parser_state));
	arena_clear(&c->arena[AT_STRING]);
	arena_clear(&c->arena[AT_TOKENS]);
	have_lookahead = false;
	while (run_parse_iteration(c, &parse, &lex, &have_lookahead))
		;
	parse.flags |= PARSE_LOOKAHEAD_EOF;
	final_pass(c, &parse, &lex);
	if (!(parse.flags & PARSE_ERROR) && (parse.flags & PARSE_SAVE_TOKENS))
		get_here_doc(c, &lex, &parse.here);
	return (parse);
}