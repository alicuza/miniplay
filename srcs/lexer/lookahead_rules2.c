#include "minishell.h"

bool	apply_rule_6(t_ctx *c, t_lexer_state *lex)
{
#ifdef DEBUG
	print_lex_rule(c, 6);
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
	print_lex_rule(c, 7);
#endif
	consume_char(lex, 1);
	if (lex->flags & LEX_IS_BUILDING)
	{
		delimit_lex_token(c, lex);
		return (true);
	}
	return (false);
}

bool	apply_rule_8(t_ctx *c, t_lexer_state *lex)
{
#ifdef DEBUG
	print_lex_rule(c, 8);
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
	print_lex_rule(c, 9);
#endif
	while (c->read_line[lex->char_idx] && c->read_line[lex->char_idx] != '\n')
		consume_char(lex, 1);
	return (false);
}

bool	apply_rule_10(t_ctx *c, t_lexer_state *lex)
{
#ifdef DEBUG
	print_lex_rule(c, 10);
#else
	(void)c;
#endif
	start_lex_token(lex, TKN_WORD);
	consume_char(lex, 1);
	return (false);
}