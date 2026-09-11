#include "minishell.h"

t_symbol	*get_symbol_from_top(t_ctx *c, uint32_t depth)
{
	t_arena	*stack;

	stack = &c->arena[AT_STACK];
	return (get_ptr_from_top(stack, depth));
}

t_symbol	*get_symbol_from_idx(t_ctx *c, uint64_t idx)
{
	t_arena	*stack;

	stack = &c->arena[AT_STACK];
	return (get_ptr_from_idx(stack, idx));
}

t_symbol	*get_symbol_from_rhs(t_ctx *c, t_parser_state *parse, t_rule *rule,
		uint32_t rhs_pos)
{
	(void)parse;
	return (get_symbol_from_top(c, rule->rhs_len - rhs_pos));
}

t_token	*get_token_from_idx(t_ctx *c, uint64_t idx)
{
	t_arena	*tokens;

	tokens = &c->arena[AT_TOKENS];
	return (get_ptr_from_idx(tokens, idx));
}

char	*get_token_body(t_ctx *c, t_token *token)
{
	t_arena	*strings;

	strings = &c->arena[AT_STRING];
	return (get_ptr_from_offset(strings, token->offset));
}
