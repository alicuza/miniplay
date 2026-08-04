/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_input.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nribakov <nribakov@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
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
			return (false);													// eof reached
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

t_parser_state	parse_input(t_ctx *c)
{
	t_parser_state	parse;
	t_lexer_state	lex;
	t_lalr_action	action;
	t_token			*cur;
	t_arena			*strings;
	t_arena			*tokens;
	bool			have_lookahead;

	ft_memset(&lex, 0, sizeof(t_lexer_state));
	ft_memset(&parse, 0, sizeof(t_parser_state));
	strings = &c->arena[AT_STRING];
	tokens = &c->arena[AT_TOKENS];
	arena_clear(strings);
	arena_clear(tokens);
	have_lookahead = false;
	while (true)
	{
		if (!have_lookahead)
		{
			if (!get_next_token(c, &parse, &lex))
			{
				if (parse.flags & PARSE_SAVE_TOKENS)		/* divert-at-EOF */
				{
					parse.flags &= ~PARSE_SAVE_TOKENS;
					parse.flags |= PARSE_HERE_BODY;
					continue ;
				}
				break ;										/* eof reached */
			}
			have_lookahead = true;
		}
		cur = get_ptr_from_idx(tokens, parse.token_idx);
#ifdef DEBUG
		fprintf(stderr, "\n--- lookahead ---\n");
		print_token(stderr, c, cur);
		print_arena(strings);
		print_arena(tokens);
#endif
		if (parse.flags & PARSE_SAVE_TOKENS)				/* divert intercept */
		{
			if (strings->buf[cur->offset] == '\n')
			{
				parse.flags &= ~PARSE_SAVE_TOKENS;
				parse.flags |= PARSE_HERE_BODY;
			}
			have_lookahead = false;							/* non-\n tokens dropped */
			continue ;
		}
		action = shift_reduce(c, &parse, &lex);
		if (action == LALR_REDUCE)
			continue ;										/* same lookahead */
		have_lookahead = false;								/* lookahead consumed */
#ifdef DEBUG
		{
			t_symbol	*symbol;

			symbol = get_ptr_from_idx(&c->arena[AT_STACK], parse.stack_idx);
			print_symbol(c, symbol, parse.stack_idx);
			print_arena(&c->arena[AT_STACK]);
		}
#endif
		if (action == LALR_ACCEPT)
		{
			parse.flags |= PARSE_DONE;
			return (parse);
		}
		if (action == LALR_ERROR)
		{
			report_parse_error(c, &parse);
			return (parse);
		}
	}
	parse.flags |= PARSE_LOOKAHEAD_EOF;						/* final pass */
	while (true)
	{
		action = shift_reduce(c, &parse, &lex);
		if (action == LALR_ACCEPT)
		{
			parse.flags |= PARSE_DONE;
			break ;
		}
		if (action == LALR_ERROR)
		{
			report_parse_error(c, &parse);
			break ;
		}
	}
	if (!(parse.flags & PARSE_ERROR) && (parse.flags & PARSE_SAVE_TOKENS))
		get_here_doc(c, &lex, &parse.here);					/* empty body + warning */
	return (parse);
}
