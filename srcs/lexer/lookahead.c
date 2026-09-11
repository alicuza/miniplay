/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lookahead.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sancuta <sancuta@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/25 13:37:40 by sancuta           #+#    #+#             */
/*   Updated: 2026/08/08 14:09:42 by sancuta          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool	lex_token(t_ctx *c, t_lexer_state *lex)
{
#ifdef DEBUG
	if (c->dbg.states & DBG_LEXER)
		print_lex_state(c, lex);
#endif
	if (!c->read_line[lex->char_idx])										// rule 1
		return (apply_rule_1(c, lex));
	else if (lex->type == TKN_OPERATOR)										// rule 2 & 3
	{
		if (is_str_in_set(c->read_line + lex->char_idx - 1, get_operator_strs()))
			return (apply_rule_2(c, lex));
		else
			return (apply_rule_3(c, lex));
	}
	else if (is_char_in_set(c->read_line[lex->char_idx], QUOTE_SET))		// rule 4
		return (apply_rule_4(c, lex));
	else if (is_expansion_start(c->read_line, lex->char_idx))				// rule 5
		return (apply_rule_5(c, lex));
	else if (is_char_in_set(c->read_line[lex->char_idx], OPERATOR_SET)		// rule 6
			&& (c->read_line[lex->char_idx] != '&'
			|| c->read_line[lex->char_idx + 1] == '&'))
		return (apply_rule_6(c, lex));
	else if (is_char_in_set(c->read_line[lex->char_idx], BLANK_SET))		// rule 7
		return (apply_rule_7(c, lex));
	else if (lex->type == TKN_WORD)											// rule 8
		return (apply_rule_8(c, lex));
	else if (c->read_line[lex->char_idx] == '#')							// rule 9
		return (apply_rule_9(c, lex));
	else																	// rule 10
		return (apply_rule_10(c, lex));
}