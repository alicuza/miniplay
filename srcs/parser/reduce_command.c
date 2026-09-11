#include "minishell.h"

uint64_t	reduce_simple_command_from_suffix(t_ctx *c, t_parser_state *parse,
		t_rule *rule)
{
	t_symbol	*suffix_sym;
	t_symbol	*name_sym;
	t_symbol	*prefix_sym;
	t_node		*cmd_node;

	suffix_sym = get_symbol_from_rhs(c, parse, rule, rule->rhs_len - 1);
	name_sym = get_symbol_from_rhs(c, parse, rule, rule->rhs_len - 2);
	cmd_node = get_node_from_idx(c, suffix_sym->node_idx);
	cmd_node->data.command.arg_head_idx = append_node_to_tail(c,
			name_sym->node_idx, cmd_node->data.command.arg_head_idx);
	if (rule->rhs_len == 3)
	{
		prefix_sym = get_symbol_from_rhs(c, parse, rule, 0);
		cmd_node->data.command.redir_head_idx = append_node_to_tail(c,
				prefix_sym->node_idx, cmd_node->data.command.redir_head_idx);
	}
	return (suffix_sym->node_idx);
}

uint64_t	reduce_simple_command(t_ctx *c, t_parser_state *parse,
		t_rule *rule)
{
	t_symbol	*lead_sym;
	t_symbol	*word_sym;
	t_node		*cmd_node;
	uint64_t	*head_idx;
	uint64_t	cmd_idx;

	cmd_idx = alloc_node(c, NODE_COMMAND);
	cmd_node = get_node_from_idx(c, cmd_idx);
	lead_sym = get_symbol_from_rhs(c, parse, rule, 0);
	head_idx = &cmd_node->data.command.arg_head_idx;
	if (lead_sym->type == SYM_CMD_PREFIX)
		head_idx = &cmd_node->data.command.redir_head_idx;
	*head_idx = lead_sym->node_idx;
	if (rule->rhs_len == 2)
	{
		word_sym = get_symbol_from_rhs(c, parse, rule, 1);
		cmd_node->data.command.arg_head_idx = word_sym->node_idx;
	}
	return (cmd_idx);
}

static void	suffix_attach_last(t_ctx *c, t_parser_state *parse,
		t_rule *rule, uint64_t cmd_idx)
{
	t_symbol	*last_sym;
	t_node		*cmd;
	uint64_t	*head_idx;
	uint64_t	child_idx;

	last_sym = get_symbol_from_rhs(c, parse, rule, rule->rhs_len - 1);
	child_idx = last_sym->node_idx;
	if (last_sym->type != SYM_IO_REDIRECT)
		child_idx = reduce_from_term(c, parse, rule);
	cmd = get_node_from_idx(c, cmd_idx);
	if (last_sym->type == SYM_IO_REDIRECT)
		head_idx = &cmd->data.command.redir_head_idx;
	else
		head_idx = &cmd->data.command.arg_head_idx;
	/* append returns the head (child_idx if the list was empty), store it back */
	*head_idx = append_node_to_tail(c, *head_idx, child_idx);
}

uint64_t	reduce_cmd_suffix(t_ctx *c, t_parser_state *parse,
		t_rule *rule)
{
	t_symbol	*prev_sym;
	uint64_t	cmd_idx;

	if (rule->rhs_len == 1)
		cmd_idx = alloc_node(c, NODE_COMMAND);
	else
	{
		prev_sym = get_symbol_from_rhs(c, parse, rule, 0);
		cmd_idx = prev_sym->node_idx;
	}
	suffix_attach_last(c, parse, rule, cmd_idx);
	return (cmd_idx);
}

uint64_t	reduce_subshell_redirects(t_ctx *c, t_parser_state *parse,
		t_rule *rule)
{
	t_symbol	*cmd_sym;
	t_symbol	*redir_sym;
	t_node		*cmd_node;

	cmd_sym = get_symbol_from_rhs(c, parse, rule, 0);
	redir_sym = get_symbol_from_rhs(c, parse, rule, 1);
	cmd_node = get_node_from_idx(c, cmd_sym->node_idx);
	cmd_node->data.command.redir_head_idx = redir_sym->node_idx;
	return (cmd_sym->node_idx);
}
