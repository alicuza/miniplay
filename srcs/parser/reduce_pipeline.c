#include "minishell.h"

uint64_t	reduce_program(t_ctx *c, t_parser_state *parse, t_rule *rule)
{
	t_symbol	*commands_sym;

	commands_sym = get_symbol_from_rhs(c, parse, rule, 1);
	return (commands_sym->node_idx);
}

uint64_t	reduce_list_append(t_ctx *c, t_parser_state *parse,
		t_rule *rule)
{
	t_symbol	*list_sym;
	t_symbol	*elem_sym;

	list_sym = get_symbol_from_rhs(c, parse, rule, 0);
	elem_sym = get_symbol_from_rhs(c, parse, rule, rule->rhs_len - 1);
	return (append_node_to_tail(c, list_sym->node_idx, elem_sym->node_idx));
}

uint64_t	reduce_and_or_conditional(t_ctx *c, t_parser_state *parse,
		t_rule *rule)
{
	t_symbol	*op_sym;
	t_symbol	*pipeline_sym;
	t_node		*pipeline_node;

	op_sym = get_symbol_from_rhs(c, parse, rule, 1);
	pipeline_sym = get_symbol_from_rhs(c, parse, rule, 3);
	pipeline_node = get_node_from_idx(c, pipeline_sym->node_idx);
	if (op_sym->type == SYM_AND_IF)
		pipeline_node->flags |= FLAG_AND_IF;
	else
		pipeline_node->flags |= FLAG_OR_IF;
	return (reduce_list_append(c, parse, rule));
}

uint64_t	reduce_pipeline_create(t_ctx *c, t_parser_state *parse,
		t_rule *rule)
{
	t_symbol	*cmd_sym;
	t_node		*pipeline_node;
	uint64_t	pipeline_idx;

	pipeline_idx = alloc_node(c, NODE_PIPELINE);
	pipeline_node = get_node_from_idx(c, pipeline_idx);
	cmd_sym = get_symbol_from_rhs(c, parse, rule, 0);
	pipeline_node->data.pipeline.command_head_idx = cmd_sym->node_idx;
	return (pipeline_idx);
}

uint64_t	reduce_pipeline_append(t_ctx *c, t_parser_state *parse,
		t_rule *rule)
{
	t_symbol	*pipeline_sym;
	t_symbol	*cmd_sym;
	t_node		*pipeline_node;

	pipeline_sym = get_symbol_from_rhs(c, parse, rule, 0);
	cmd_sym = get_symbol_from_rhs(c, parse, rule, 3);
	pipeline_node = get_node_from_idx(c, pipeline_sym->node_idx);
	pipeline_node->data.pipeline.command_head_idx = append_node_to_tail(c,
			pipeline_node->data.pipeline.command_head_idx, cmd_sym->node_idx);
	return (pipeline_sym->node_idx);
}
