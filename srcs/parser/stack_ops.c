#include "minishell.h"

static uint64_t	alloc_symbol(t_ctx *c)
{
	t_arena	*stack;

	stack = &c->arena[AT_STACK];
	return (get_idx_from_offset(stack,
			arena_alloc(stack, sizeof(t_symbol), _Alignof(t_symbol))));
}

void	push_symbol(t_ctx *c, t_parser_state *parse, t_symbol *src)
{
	t_symbol	*symbol;

	parse->stack_idx = alloc_symbol(c);
	symbol = get_symbol_from_idx(c, parse->stack_idx);
	symbol->type = src->type;
	symbol->token_idx = src->token_idx;
	symbol->node_idx = src->node_idx;
	symbol->entry_state = parse->state;
}

void	pop_symbols(t_ctx *c, t_parser_state *parse, uint32_t len)
{
	t_arena		*stack;
	t_symbol	*symbol;

	stack = &(c->arena[AT_STACK]);
	parse->stack_idx -= len;
	stack->offset -= len * stack->stride;
	symbol = get_ptr_from_idx(stack, parse->stack_idx);
	parse->state = symbol->entry_state;
}

void	reduce_apply_goto(t_parser_state *parse, t_rule *rule)
{
	int32_t	lhs;
	int32_t	index;

	lhs = rule->lhs_type - NTERM_OFFSET;
	index = get_yypgoto(lhs) + parse->state;
	if (0 <= index && index <= YYLAST && get_yycheck(index) == parse->state)
		parse->state = get_yytable(index);
	else
		parse->state = get_yydefgoto(lhs);
}
