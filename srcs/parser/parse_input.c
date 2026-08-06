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
			return (false);
		}
		if (lex_token(c, lex))
		{
			++parse->token_idx;
			return (true);
		}
	}
}

t_parser_state	parse_input(t_ctx *c)
{
	t_parser_state	parse;
	t_lexer_state	lex;
	t_lalr_action	action;
	t_token			*lookahead;
	t_arena			*strings;
	t_arena			*tokens;

	ft_memset(&lex, 0, sizeof(t_lexer_state));
	ft_memset(&parse, 0, sizeof(t_parser_state));
	strings = &c->arena[AT_STRING];
	tokens = &c->arena[AT_TOKENS];
	arena_clear(strings);
	arena_clear(tokens);
	while (true)
	{
		if (!(parse.flags & PARSE_HAS_LOOKAHEAD))				/* start or reduction happened */
		{
			if (!get_next_token(c, &parse, &lex))				/* EOF reached */
			{
				if (parse.flags & PARSE_SAVE_TOKENS)			/* io_here was reduced, detour to save the remaining non-'\n'-tokens on the line */
				{
					parse.flags &= ~PARSE_SAVE_TOKENS;
					parse.flags |= PARSE_HERE_BODY;
					continue ;
				}
				parse.flags |= PARSE_LOOKAHEAD_IS_EOF;
				break ;
			}
			parse.flags |= PARSE_HAS_LOOKAHEAD;
		}
		lookahead = get_ptr_from_idx(tokens, parse.token_idx);	/* either just written into the arena by get_next_token or from having been saved previously */
#ifdef DEBUG
		FILE *out = stderr;
		if (c->scope & SCOPE_TOKENS)
			out = stdout;
		fprintf(stderr, "\n--- lookahead ---\n");
		print_token(out, c, lookahead);
		print_arena(strings);
		print_arena(tokens);
#endif
		if (parse.flags & PARSE_SAVE_TOKENS)					/* io_here was reduced, detour to save the remaining non-'\n'-tokens on the line */
		{
			if (strings->buf[lookahead->offset] == '\n')		/* '\n' at the end of line reached, continue detour and start collecting the here_body */
			{
				parse.flags &= ~PARSE_SAVE_TOKENS;
				parse.flags |= PARSE_HERE_BODY;
			}
			parse.flags &= ~PARSE_HAS_LOOKAHEAD;
			continue ;
		}
		action = shift_reduce(c, &parse);
		if (action == LALR_REDUCE)								/* REDUCE doesn't consume lookahead */
			continue ;
		parse.flags &= ~PARSE_HAS_LOOKAHEAD;					/* SHIFT consumes lookahead */
#ifdef DEBUG
		{
			t_symbol	*symbol;

			symbol = get_ptr_from_idx(&c->arena[AT_STACK], parse.stack_idx);
			print_symbol(c, symbol, parse.stack_idx);
			print_arena(&c->arena[AT_STACK]);
		}
#endif
		if (action == LALR_ERROR)
		{
			parse.flags |= PARSE_ERROR;
			ft_putstr_fd("minishell: syntax error near unexpected token'\n", 2);
			return (parse);
		}
	}
	while (true)												/* working through the synthetic eof lookahead */
	{
		action = shift_reduce(c, &parse);
		if (action == LALR_ACCEPT)
		{
			parse.flags |= PARSE_DONE;
			break ;
		}
		if (action == LALR_ERROR)
		{
			parse.flags |= PARSE_ERROR;
			ft_putstr_fd("minishell: syntax error near unexpected token'\n", 2);
			break ;
		}
	}
	return (parse);
}
