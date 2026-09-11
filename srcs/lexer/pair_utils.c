/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pair_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sancuta <sancuta@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/21 19:59:39 by sancuta           #+#    #+#             */
/*   Updated: 2026/07/22 13:17:37 by sancuta          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	matching_close(char open)
{
	char	close;

	close = open;
	if (open == '(')
		close = ')';
	return (close);
}

bool	find_matched_pair(t_ctx *c, t_lexer_state *lex, char open)
{
    t_slice	save;
    char	close;
    bool	is_dquote;

    close = matching_close(open);
    is_dquote = (open == '"');
    save = save_lex_token_slice(lex);
    consume_char(lex, 1);
    while (c->read_line[lex->char_idx]
		&& c->read_line[lex->char_idx] != close)
    {
		if (is_dquote && !(lex->flags & TKN_HAS_EXPANSION)
			&& is_expansion_start(c->read_line, lex->char_idx))
			lex->flags |= TKN_HAS_EXPANSION;
        grow_lex_token(lex, 1);
        consume_char(lex, 1);
    }
    if (!c->read_line[lex->char_idx])
    {
		restore_lex_token_slice(lex, save);
        return (false);
    }
    grow_lex_token(lex, 1);
	consume_char(lex, 1);
	lex->flags |= TKN_HAS_QUOTES;
    return (true);
}
