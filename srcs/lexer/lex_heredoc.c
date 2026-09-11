/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lex_heredoc.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sancuta <sancuta@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/26 13:03:24 by sancuta           #+#    #+#             */
/*   Updated: 2026/08/13 13:00:14 by sancuta          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	handle_here_body(t_ctx *c, t_parser_state *parse, t_lexer_state *lex)
{
#ifdef DEBUG
	if (c->dbg.states & DBG_HEREDOC)
		print_here_reading(c, parse);
#endif
	get_here_doc(c, lex);
	parse->flags &= ~PARSE_HERE_BODY;
	parse->token_idx = get_symbol_from_top(c, 1)->token_idx;
#ifdef DEBUG
	if (c->dbg.states & DBG_HEREDOC)
		print_here_stored(parse, parse->token_idx);
#endif
}

void	handle_saved_tokens(t_ctx *c, t_parser_state *parse)
{
	t_arena	*tokens;
	t_token	*next;
	char	*body;

	tokens = &c->arena[AT_TOKENS];
	next = get_ptr_from_idx(tokens, parse->token_idx + 1);
	body = get_token_body(c, next);
#ifdef DEBUG
	if (c->dbg.states & DBG_HEREDOC)
		print_here_replay(parse->token_idx + 1);
#endif
	if (*body == '\n')
		parse->flags &= ~PARSE_HAS_SAVED_TOKENS;
	++parse->token_idx;
}
