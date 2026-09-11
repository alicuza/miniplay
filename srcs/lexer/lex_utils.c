/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lex_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sancuta <sancuta@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/25 13:03:51 by sancuta           #+#    #+#             */
/*   Updated: 2026/09/11 12:40:50 by sancuta          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

uint64_t	consume_char(t_lexer_state *lex, uint64_t len)
{
	lex->char_idx += len;
	return (lex->char_idx);
}

t_slice	save_lex_token_slice(t_lexer_state *lex)
{
	t_slice	save;

	save.pos = lex->char_idx;
	save.len = lex->token.len;
	return (save);
}

void	restore_lex_token_slice(t_lexer_state *lex, t_slice saved)
{
	lex->char_idx = saved.pos;
	lex->token.len = saved.len;
}

const char	**get_operator_strs(void)
{
	static const char	*set[] = {"<<", ">>", "&&", "||", NULL};

	return (set);
}
