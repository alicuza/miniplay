#include "minishell.h"

bool	apply_rule_1(t_ctx *c, t_lexer_state *lex)
{
#ifdef DEBUG
	print_lex_rule(c, 1);
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
	print_lex_rule(c, 2);
#endif
	if (lex->flags & LEX_IS_BUILDING)
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
	print_lex_rule(c, 3);
#endif
	delimit_lex_token(c, lex);
	return (true);
}

bool	apply_rule_4(t_ctx *c, t_lexer_state *lex)
{
#ifdef DEBUG
	print_lex_rule(c, 4);
#endif
	if (lex->flags & LEX_IS_BUILDING)
		grow_lex_token(lex, 1);
	else
		start_lex_token(lex, TKN_WORD);
	if (!find_matched_pair(c, lex, c->read_line[lex->char_idx]))
		consume_char(lex, 1);
	return (false);
}

bool	apply_rule_5(t_ctx *c, t_lexer_state *lex)
{
	uint64_t	len;
#ifdef DEBUG
	print_lex_rule(c, 5);
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