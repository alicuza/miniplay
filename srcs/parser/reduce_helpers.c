#include "minishell.h"

uint64_t	reduce_compute_result(t_ctx *c, t_parser_state *parse,
		t_rule *rule, uint64_t *token_idx)
{
	uint64_t	node_idx;

	if (rule->rhs_len)
	{
		if (rule->handler)
			node_idx = rule->handler(c, parse, rule);
		else
			node_idx = get_symbol_from_rhs(c, parse, rule, 0)->node_idx;
		*token_idx = get_symbol_from_rhs(c, parse, rule,
			rule->rhs_len - 1)->token_idx;
#ifdef DEBUG
		if ((c->dbg.states & DBG_PARSER) && (c->dbg.parser & DBG_SHOW_LINKS))
			log_rhs_symbols(c, parse, rule);
#endif
	}
	else
	{
		node_idx = 0;
		*token_idx = parse->token_idx;
	}
	return (node_idx);
}

uint64_t	get_exec_root(t_ctx *c, t_parser_state *parse, t_rule *rule)
{
	if (rule->lhs_type != SYM_COMPLETE_COMMANDS)
		return (0);
	return (get_symbol_from_rhs(c, parse, rule,
			rule->rhs_len - 1)->node_idx);
}

void	init_parser(t_ctx *c, t_parser_state *parse, t_lexer_state *lex)
{
	ft_memset(parse, 0, sizeof(t_parser_state));
	ft_memset(lex, 0, sizeof(t_lexer_state));
	arena_clear(&c->arena[AT_STRING]);
	arena_clear(&c->arena[AT_TOKENS]);
	arena_clear(&c->arena[AT_STACK]);
	arena_clear(&c->arena[AT_COMMAND]);
}
