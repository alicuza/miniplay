#include "minishell.h"

uint64_t	reduce_from_term(t_ctx *c, t_parser_state *parse, t_rule *rule)
{
	t_symbol	*symbol;
	t_token		*token;
	t_node		*node;
	t_node_type	node_type;
	uint64_t	node_idx;

	node_type = NODE_ARG;
	if (rule->lhs_type == SYM_FILENAME)
		node_type = NODE_REDIR;
	symbol = get_symbol_from_rhs(c, parse, rule, rule->rhs_len - 1);
	token = get_token_from_idx(c, symbol->token_idx);
	node_idx = alloc_node(c, node_type);
	node = get_node_from_idx(c, node_idx);
	if (node_type == NODE_REDIR)
	{
		node->data.redir.arena_offset = token->offset;
		node->data.redir.flags = token->flags & 0x03;
	}
	else
	{
		node->data.arg.arena_offset = token->offset;
		node->data.arg.flags = token->flags & 0x03;
	}
	return (node_idx);
}
