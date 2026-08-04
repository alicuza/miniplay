/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lookahead.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sancuta <sancuta@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/25 13:37:40 by sancuta           #+#    #+#             */
/*   Updated: 2026/07/26 17:38:04 by sancuta          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool	apply_rule_1(t_ctx *c, t_lexer_state *lex)
{
#ifdef DEBUG
	if (c->states & DBG_LEXER)
		fprintf(stderr, "rule 1\n");
#endif
	lex->flags |= LEX_AT_EOI;
	if (lex->flags & LEX_IS_BUILDING)
	{
		delimit_lex_token(c, lex);
		return (true);
	}
	return (false);
}

bool	apply_rule_2(t_ctx *c, t_lexer_state *lex)
{
#ifdef DEBUG
	if (c->states & DBG_LEXER)
		fprintf(stderr, "rule 2\n");
#endif
	if(lex->flags & LEX_IS_BUILDING)
	{
		consume_char(lex, 1);
		grow_lex_token(lex, 1);
		delimit_lex_token(c, lex);
		return (true);
	}
	return (false);
}

bool	apply_rule_3(t_ctx *c, t_lexer_state *lex)
{
#ifdef DEBUG
	if (c->states & DBG_LEXER)
		fprintf(stderr, "rule 3\n");
#endif
	delimit_lex_token(c, lex);
	return (true);
}

bool	apply_rule_4(t_ctx *c, t_lexer_state *lex)
{
#ifdef DEBUG
	if (c->states & DBG_LEXER)
		fprintf(stderr, "rule 4\n");
#endif
	if (lex->flags & LEX_IS_BUILDING)
		grow_lex_token(lex, 1);
	else
		start_lex_token(lex, TKN_WORD);
	lex->pair.open = c->read_line[lex->char_idx];
	lex->pair.close = matching_close(lex->pair.open);
	if (!find_matched_pair(c, lex))
	{
		lex->pair.open = 0;
		lex->pair.close = 0;
		consume_char(lex, 1);
	}
	return (false);
}

bool	apply_rule_5(t_ctx *c, t_lexer_state *lex)
{
	uint64_t	len;
#ifdef DEBUG
	if (c->states & DBG_LEXER)
		fprintf(stderr, "rule 5\n");
#endif
	len = get_expansion_len(c->read_line + lex->char_idx);
	if (!(lex->flags & LEX_IS_BUILDING))
	{
		start_lex_token(lex, TKN_WORD);
		grow_lex_token(lex, len - 1);
	}
	else
		grow_lex_token(lex, len);
	consume_char(lex, len);
	lex->flags |= TKN_HAS_EXPANSION;
	return (false);
}

bool	apply_rule_6(t_ctx *c, t_lexer_state *lex)
{
#ifdef DEBUG
	if (c->states & DBG_LEXER)
		fprintf(stderr, "rule 6\n");
#endif
	if (lex->flags & LEX_IS_BUILDING)
	{
		delimit_lex_token(c, lex);
		return (true);
	}
	start_lex_token(lex, TKN_OPERATOR);
	consume_char(lex, 1);
	return (false);
}

bool	apply_rule_7(t_ctx *c, t_lexer_state *lex)
{
#ifdef DEBUG
	if (c->states & DBG_LEXER)
		fprintf(stderr, "rule 7\n");
#endif
	consume_char(lex, 1);
	if(lex->flags & LEX_IS_BUILDING)
	{
		delimit_lex_token(c, lex);
		return (true);
	}
	return (false);
}

bool	apply_rule_8(t_ctx *c, t_lexer_state *lex)
{
#ifdef DEBUG
	if (c->states & DBG_LEXER)
		fprintf(stderr, "rule 8\n");
#else
	(void)c;
#endif
	grow_lex_token(lex, 1);
	consume_char(lex, 1);
	return (false);
}

bool	apply_rule_9(t_ctx *c, t_lexer_state *lex)
{
#ifdef DEBUG
	if (c->states & DBG_LEXER)
		fprintf(stderr, "rule 9\n");
#endif
	while (c->read_line[lex->char_idx] && c->read_line[lex->char_idx] != '\n')
		consume_char(lex, 1);
	return (false);
}

bool	apply_rule_10(t_ctx *c, t_lexer_state *lex)
{
#ifdef DEBUG
	if (c->states & DBG_LEXER)
		fprintf(stderr, "rule 10\n");
#else
	(void)c;
#endif
	start_lex_token(lex, TKN_WORD);
	consume_char(lex, 1);
	return (false);
}

bool	lex_token(t_ctx *c, t_lexer_state *lex)
{
#ifdef DEBUG
	if (c->states & DBG_LEXER)
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
