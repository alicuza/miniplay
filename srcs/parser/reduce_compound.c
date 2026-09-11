#include "minishell.h"

uint64_t	reduce_compound_list(t_ctx *c, t_parser_state *parse,
		t_rule *rule)
{
	t_symbol	*term_sym;

	term_sym = get_symbol_from_rhs(c, parse, rule, 1);
	return (term_sym->node_idx);
}

uint64_t	reduce_subshell(t_ctx *c, t_parser_state *parse, t_rule *rule)
{
	t_symbol	*compound_sym;
	t_node		*cmd_node;
	uint64_t	cmd_idx;

	cmd_idx = alloc_node(c, NODE_COMMAND);
	cmd_node = get_node_from_idx(c, cmd_idx);
	compound_sym = get_symbol_from_rhs(c, parse, rule, 1);
	cmd_node->flags |= FLAG_SUBSHELL;
	cmd_node->data.command.arg_head_idx = compound_sym->node_idx;
	return (cmd_idx);
}
