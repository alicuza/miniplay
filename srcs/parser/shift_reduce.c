#include "minishell.h"

#define YYPACT_NINF (-44)	/* stands for negative infinity */
#define YYFINAL      5
#define YYLAST       67
#define NTERM_OFFSET 14

/* -------- LALR tables, regenerated from shni_grammar_reduced.y ----------- */
static const int32_t yypact[] =
{
	  8,  -44,   18,   21,   33,  -44,  -44,  -44,   14,   14,
	 30,   14,    8,    8,   17,   16,  -44,   58,  -44,   40,
	 47,  -44,  -44,  -44,  -44,  -44,  -44,  -44,  -44,  -44,
	 24,   33,    7,  -44,    8,    8,    8,   58,  -44,  -44,
	 54,  -44,  -44,   40,  -44,  -44,   17,    8,   17,   33,
	 33,   33,  -44,  -44,  -44,   54,    7,   16,   16,  -44,
	 17
};

static const int32_t yydefact[] =
{
	 45,   42,    0,   44,    3,    1,   43,   24,    0,    0,
	  0,    0,   45,   45,    5,    6,    9,   12,   11,   23,
	 21,   26,   34,   35,   39,   36,   37,   41,   40,   38,
	  0,    0,   44,    2,   45,   45,   45,   13,   32,   30,
	 22,   28,   25,   20,   27,   14,   18,   15,    4,    0,
	  0,    0,   33,   31,   29,   19,   16,    7,    8,   10,
	 17
};

static const int32_t yypgoto[] =
{
	-44,  -44,  -44,  -27,  -43,   -7,  -44,  -44,  -44,  -44,
	-44,  -44,  -44,   -9,  -44,  -17,  -44,   13,  -44,  -44,
	-12,   -4
};

static const int32_t yydefgoto[] =
{
	  0,    2,   13,   14,   15,   16,   17,   30,   47,   18,
	 19,   43,   20,   40,   37,   21,   22,   25,   23,   28,
	  3,    4
};

static const int32_t yytable[] =
{
	 38,   32,   41,   44,   46,   48,   57,   58,   31,   33,
	  7,    6,    1,    8,    9,   10,   11,   24,    5,   12,
	 52,   36,   26,   54,   29,    6,   41,   34,   35,   60,
	 49,   50,   51,   27,   55,   56,    7,   45,   54,    8,
	  9,   10,   11,   39,   59,   12,    8,    9,   10,   11,
	 42,    0,    0,    8,    9,   10,   11,   53,    0,    0,
	  8,    9,   10,   11,    8,    9,   10,   11
};

static const int32_t yycheck[] =
{
	 17,   13,   19,   20,   31,   32,   49,   50,   12,   13,
	  3,    4,    4,    6,    7,    8,    9,    3,    0,   12,
	 37,    5,    9,   40,   11,    4,   43,   10,   11,   56,
	 34,   35,   36,    3,   43,   47,    3,   13,   55,    6,
	  7,    8,    9,    3,   51,   12,    6,    7,    8,    9,
	  3,   -1,   -1,    6,    7,    8,    9,    3,   -1,   -1,
	  6,    7,    8,    9,    6,    7,    8,    9
};

/* -------- node arena helpers ---------------------------------------------- */
uint64_t	alloc_node(t_ctx *c, t_node_type type)
{
	t_arena		*commands;
	t_node		*node;
	uint64_t	node_idx;

	commands = &c->arena[AT_COMMAND];
	node_idx = get_idx_from_offset(commands,
			arena_alloc(commands, sizeof(t_node), _Alignof(t_node)));
	node = get_ptr_from_idx(commands, node_idx);
	node->type = type;
	return (node_idx);
}

// TODO stefan: consider inlining the short wrappers
t_node	*get_node_from_idx(t_ctx *c, uint64_t idx)
{
	t_arena	*commands;

	commands = &c->arena[AT_COMMAND];
	return (get_ptr_from_idx(commands, idx));
}

void	*get_ptr_from_top(t_arena *arena, uint32_t pos)
{
	uint64_t	offset;

	offset = arena->offset - pos * arena->stride;
	return (get_ptr_from_offset(arena, offset));
}

t_token	*get_token_from_idx(t_ctx *c, uint64_t idx)
{
	t_arena	*tokens;

	tokens = &c->arena[AT_TOKENS];
	return (get_ptr_from_idx(tokens, idx));
}

t_symbol	*get_symbol_from_rhs(t_ctx *c, t_rule *rule, uint32_t rhs_pos)
{
	t_arena	*stack;

	stack = &c->arena[AT_STACK];
	return (get_ptr_from_top(stack, rule->rhs_len - rhs_pos));
}

uint64_t	get_node_tail_idx(t_ctx *c, uint64_t head_idx)
{
	t_node		*node;
	uint64_t	tail_idx;

	node = get_node_from_idx(c, head_idx);
	tail_idx = head_idx;
	while (node->next_idx)
	{
		tail_idx = node->next_idx;
		node = get_node_from_idx(c, tail_idx);
	}
	return (tail_idx);
}

uint64_t	append_node_to_tail(t_ctx *c, uint64_t head_idx, uint64_t new_idx)
{
	t_node		*tail;
	uint64_t	tail_idx;

	if (!head_idx)		// TODO stefan: check if this is ever used, or if we branch in the caller.
		return (new_idx);
	tail_idx = get_node_tail_idx(c, head_idx);
	tail = get_node_from_idx(c, tail_idx);
	tail->next_idx = new_idx;
	return (new_idx);
}
/* TODO stefan: decide if i want to have this return the idx or write it, or both.
uint64_t	construct_node_from_terminal(t_ctx *c, uint64_t from_top)
{
	t_token		*token;
	t_symbol	*symbol;
	t_node		*node;
	uint64_t	node_idx;

	symbol = get_symbol_from_top(c, from_top);
	token = get_token_from_idx(c, symbol->token_idx);
	node_idx = alloc_node(c, NODE_DEFAULT);
	node = get_node_from_idx(c, node_idx);
	node->data.arg.arena_offset = token->offset;
	return (node_idx);
}
*/
/* -------- reduction handlers (index == bison rule number) ------------------ */

static const t_rule	rules[RULE_COUNT] =
{
	[0] = {NULL, 0, SYM_EOF},
	[1] = {NULL, 2, SYM_ACCEPT},
	[2] = {reduce_program, 3, SYM_PROGRAM},
	[3] = {NULL, 1, SYM_PROGRAM},
	[4] = {reduce_complete_commands, 3, SYM_COMPLETE_COMMANDS},
	[5] = {NULL, 1, SYM_COMPLETE_COMMANDS},
	[6] = {NULL, 1, SYM_AND_OR},
	[7] = {reduce_list_and_if, 4, SYM_AND_OR},
	[8] = {reduce_list_or_if, 4, SYM_AND_OR},
	[9] = {reduce_pipeline_create, 1, SYM_PIPELINE},
	[10] = {reduce_pipeline_append, 4, SYM_PIPELINE},
	[11] = {NULL, 1, SYM_COMMAND},
	[12] = {NULL, 1, SYM_COMMAND},
	[13] = {NULL, 2, SYM_COMMAND},
	[14] = {reduce_subshell, 3, SYM_SUBSHELL},
	[15] = {NULL, 2, SYM_COMPOUND_LIST},
	[16] = {NULL, 3, SYM_COMPOUND_LIST},
	[17] = {NULL, 3, SYM_TERM},
	[18] = {NULL, 1, SYM_TERM},
	[19] = {reduce_simple_command, 3, SYM_SIMPLE_COMMAND},
	[20] = {reduce_simple_command, 2, SYM_SIMPLE_COMMAND},
	[21] = {reduce_simple_command, 1, SYM_SIMPLE_COMMAND},
	[22] = {reduce_simple_command, 2, SYM_SIMPLE_COMMAND},
	[23] = {reduce_simple_command, 1, SYM_SIMPLE_COMMAND},
	[24] = {reduce_cmd_name, 1, SYM_CMD_NAME},
	[25] = {reduce_cmd_word, 1, SYM_CMD_WORD},
	[26] = {NULL, 1, SYM_CMD_PREFIX},
	[27] = {reduce_chain_append, 2, SYM_CMD_PREFIX},
	[28] = {NULL, 1, SYM_CMD_SUFFIX},
	[29] = {reduce_chain_append, 2, SYM_CMD_SUFFIX},
	[30] = {reduce_cmd_arg, 1, SYM_CMD_SUFFIX},
	[31] = {reduce_cmd_arg_append, 2, SYM_CMD_SUFFIX},
	[32] = {NULL, 1, SYM_REDIRECT_LIST},
	[33] = {reduce_chain_append, 2, SYM_REDIRECT_LIST},
	[34] = {NULL, 1, SYM_IO_REDIRECT},
	[35] = {NULL, 1, SYM_IO_REDIRECT},
	[36] = {reduce_io_file_LESS, 2, SYM_IO_FILE},
	[37] = {reduce_io_file_GREAT, 2, SYM_IO_FILE},
	[38] = {reduce_io_file_DGREAT, 2, SYM_IO_FILE},
	[39] = {reduce_filename, 1, SYM_FILENAME},
	[40] = {reduce_io_here, 2, SYM_IO_HERE},
	[41] = {NULL, 1, SYM_HERE_END},
	[42] = {NULL, 1, SYM_NEWLINE_LIST},
	[43] = {NULL, 2, SYM_NEWLINE_LIST},
	[44] = {NULL, 1, SYM_LINEBREAK},
	[45] = {NULL, 0, SYM_LINEBREAK},
};

static t_rule	get_rule(int32_t action)
{
	return (rules[action]);
}

/* -------- push / pop ------------------------------------------------------- */

uint64_t	alloc_symbol(t_ctx *c)
{
	t_arena		*stack;
	uint64_t	symbol_idx;

	stack = &c->arena[AT_STACK];
	symbol_idx = get_idx_from_offset(stack,
			arena_alloc(stack, sizeof(t_symbol), _Alignof(t_symbol)));
	return(symbol_idx);
}

static void	push_symbol(t_ctx *c, t_parser_state *parse, t_symbol symbol)
{
	t_arena		*stack;
	t_symbol	*slot;
	uint64_t	symbol_idx;

	symbol_idx = alloc_symbol(c);
	slot = get_symbol_from_idx(c, symbol_idx);
	slot->type = symbol.type;
	slot->node_idx = symbol.node_idx;
	slot->token_idx = symbol.token_idx;
	slot->entry_state = symbol.entry_state;
	parse->stack_idx = symbol_idx;
}

static void	push_nonterm(t_ctx *c, t_parser_state *parse, t_symbol_type type,
		uint64_t node_idx, uint64_t token_idx)
{
	t_symbol	*symbol;
	t_arena		*stack;

	stack = &(c->arena[AT_STACK]);
	parse->stack_idx = get_idx_from_offset(stack,
			arena_alloc(stack, sizeof(t_symbol), _Alignof(t_symbol)));
	symbol = get_ptr_from_idx(stack, parse->stack_idx);
}
      if (rule.lhs_type == SYM_COMPLETE_COMMANDS)   /* a complete_command was recognized */
      {
              parse->exec_idx = node_idx;
              return (LALR_EXEC);
      }

static void	pop(t_ctx *c, t_parser_state *parse, uint32_t len)
{
	t_arena		*stack;
	t_symbol	*symbol;

	stack = &(c->arena[AT_STACK]);
	parse->stack_idx -= len;
	stack->offset -= len * stack->stride;
	symbol = get_ptr_from_offset(stack, stack->offset - stack->stride);
	parse->state = symbol->entry_state;
}

/* -------- LALR driver ------------------------------------------------------ */
static t_lalr_action	shift(t_ctx *c, t_parser_state *parse, int32_t action)
{
	t_symbol	symbol;
	t_token		*token;

	parse->state = action;
	if (parse->state == YYFINAL)
		return (LALR_ACCEPT);
	token = get_token_from_idx(c, parse->token_idx);
	symbol.type = classify_token(c, token);
	symbol.token_idx = parse->token_idx;
	symbol.entry_state = parse->state;
	symbol.node_idx = 0;
	push_symbol(c, parse, symbol);
	return (LALR_SHIFT);
}

static uint64_t	get_lhs_node_idx(t_ctx *c, t_parser_state *parse, t_rule *rule)
{
	t_symbol	*symbol;

	if (!rule->rhs_len)
		return (0);
	if (rule->handler)
		return (rule->handler(c, parse, rule));
	symbol = get_symbol_from_rhs(c, rule, 0);
	return (symbol->node_idx);
}

static uint64_t	get_lhs_token_idx(t_ctx *c, t_parser_state *parse, t_rule *rule)
{
	t_symbol	*symbol;

	if (!rule->rhs_len)
		return (parse->token_idx);
	symbol = get_symbol_from_rhs(c, rule, rule->rhs_len - 1);
	return (symbol->token_idx);
}

int32_t	goto_state(t_parser_state *parse, t_symbol_type lhs_type)
{
	int32_t	lhs_idx;
	int32_t	index;

	lhs_idx = lhs_type - NTERM_OFFSET;
	index = yypgoto[lhs_idx] + parse->state;
	if (0 <= index && index <= YYLAST && yycheck[index] == parse->state)
		return (yytable[index]);
	return (yydefgoto[lhs_idx]);
}

static t_lalr_action	reduce(t_ctx *c, t_parser_state *parse, int32_t action)
{
	t_rule		rule;
	t_symbol	nonterm;
	uint64_t	node_idx;
	uint64_t	token_idx;

	rule = get_rule(action);
	node_idx = get_lhs_node_idx(c, parse, &rule);
	token_idx = get_lhs_token_idx(c, parse, &rule);
	pop(c, parse, rule.rhs_len);
	parse->state = goto_state(parse, rule.lhs_type);
	nonterm.type = rule.lhs_type;
	nonterm.node_idx = node_idx;
	nonterm.token_idx = token_idx;
	nonterm.entry_state = parse->state;
	push_symbol(c, parse, nonterm);
	if (parse->state == YYFINAL)
		return (LALR_ACCEPT);
	return (LALR_REDUCE);
}

t_lalr_action	shift_reduce(t_ctx *c, t_parser_state *parse)
{
	int32_t		action;
	int32_t		index;
	int32_t		lookahead_type;

	index = yypact[parse->state];
	if (index == YYPACT_NINF)
	{
		action = yydefact[parse->state];
		if (!action)
			return (LALR_ERROR);
		return (reduce(c, parse, action));
	}
	if (parse->flags & PARSE_LOOKAHEAD_IS_EOF)
		lookahead_type = SYM_EOF;
	else
		lookahead_type = classify_token(c, get_token_from_idx(c, parse->token_idx));
	index += lookahead_type;
	if (index < 0 || index > YYLAST || yycheck[index] != lookahead_type)
	{
		action = yydefact[parse->state];
		if (!action)
			return (LALR_ERROR);
		return (reduce(c, parse, action));
	}
	action = yytable[index];
	if (action <= 0)
	{
		if (!action)
			return (LALR_ERROR);
		return (reduce(c, parse, -action));
	}
	return (shift(c, parse, action));
}
