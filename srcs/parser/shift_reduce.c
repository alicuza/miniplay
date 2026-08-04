#include "minishell.h"

#define YYPACT_NINF (-44)
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
static uint64_t	node_alloc(t_ctx *c, t_node_type type)
{
	t_arena	*commands;
	t_node	*node;
	uint64_t	idx;

	commands = &c->arena[AT_COMMAND];
	idx = get_idx_from_offset(commands,
			arena_alloc(commands, sizeof(t_node), _Alignof(t_node)));
	node = get_ptr_from_idx(commands, idx);
	node->type = type;
	node->flags = 0;
	ft_memset(&node->data, 0, sizeof(node->data));
	return (idx);
}

static t_node	*node_at(t_ctx *c, uint64_t idx)
{
	return (get_ptr_from_idx(&c->arena[AT_COMMAND], idx));
}

static uint64_t	node_index(t_ctx *c, t_node *node)
{
	return (get_idx_from_offset(&c->arena[AT_COMMAND],
			(uint64_t)((char *)node - c->arena[AT_COMMAND].buf)));
}

static t_symbol	*stack_at(t_ctx *c, t_parser_state *parse, t_rule *rule,
		uint32_t i)
{
	t_arena	*stack;

	(void)parse;
	stack = &c->arena[AT_STACK];
	return (get_ptr_from_offset(stack,
			stack->offset - (rule->rhs_len - i) * stack->stride));
}

static uint64_t	chain_append(t_ctx *c, uint64_t head, uint64_t new)
{
	t_node	*node;

	if (!new)
		return (head);
	if (!head)
		return (new);
	node = node_at(c, head);
	while (node->data.arg.next)
		node = node_at(c, node->data.arg.next);
	node->data.arg.next = new;
	return (head);
}

static uint64_t	arg_leaf(t_ctx *c, t_parser_state *parse, t_rule *rule)
{
	t_symbol	*symbol;
	t_token		*token;
	uint64_t	idx;

	symbol = stack_at(c, parse, rule, rule->rhs_len - 1);
	token = get_ptr_from_idx(&c->arena[AT_TOKENS], symbol->token_idx);
	idx = node_alloc(c, NODE_ARG);
	node_at(c, idx)->data.arg.arena_offset = token->offset;
	return (idx);
}

static uint64_t	redir_leaf(t_ctx *c, t_parser_state *parse, t_rule *rule)
{
	t_symbol	*symbol;
	t_token		*token;
	uint64_t	idx;

	symbol = stack_at(c, parse, rule, rule->rhs_len - 1);
	token = get_ptr_from_idx(&c->arena[AT_TOKENS], symbol->token_idx);
	idx = node_alloc(c, NODE_REDIR);
	node_at(c, idx)->data.redir.arena_offset = token->offset;
	return (idx);
}

/* split a mixed arg/redir chain into two homogeneous chains */
static void	split_suffix(t_ctx *c, uint64_t suffix, uint64_t *args,
		uint64_t *redirs)
{
	t_node		*node;
	uint64_t	cur;

	cur = suffix;
	while (cur)
	{
		node = node_at(c, cur);
		cur = node->data.arg.next;
		node->data.arg.next = 0;
		if (node->type == NODE_ARG)
			*args = chain_append(c, *args, node_index(c, node));
		else
			*redirs = chain_append(c, *redirs, node_index(c, node));
	}
}

/* -------- reduction handlers (index == bison rule number) ------------------ */
static t_lalr_action	reduce(t_ctx *c, t_parser_state *parse, int32_t action);
static uint64_t	reduce_program(t_ctx *c, t_parser_state *parse, t_rule *rule);
static uint64_t	reduce_complete_commands(t_ctx *c, t_parser_state *parse, t_rule *rule);
static uint64_t	reduce_list_and_if(t_ctx *c, t_parser_state *parse, t_rule *rule);
static uint64_t	reduce_list_or_if(t_ctx *c, t_parser_state *parse, t_rule *rule);
static uint64_t	reduce_pipeline_create(t_ctx *c, t_parser_state *parse, t_rule *rule);
static uint64_t	reduce_pipeline_append(t_ctx *c, t_parser_state *parse, t_rule *rule);
static uint64_t	reduce_simple_command(t_ctx *c, t_parser_state *parse, t_rule *rule);
static uint64_t	reduce_cmd_name(t_ctx *c, t_parser_state *parse, t_rule *rule);
static uint64_t	reduce_cmd_word(t_ctx *c, t_parser_state *parse, t_rule *rule);
static uint64_t	reduce_cmd_arg(t_ctx *c, t_parser_state *parse, t_rule *rule);
static uint64_t	reduce_cmd_arg_append(t_ctx *c, t_parser_state *parse, t_rule *rule);
static uint64_t	reduce_chain_append(t_ctx *c, t_parser_state *parse, t_rule *rule);
static uint64_t	reduce_io_file_LESS(t_ctx *c, t_parser_state *parse, t_rule *rule);
static uint64_t	reduce_io_file_GREAT(t_ctx *c, t_parser_state *parse, t_rule *rule);
static uint64_t	reduce_io_file_DGREAT(t_ctx *c, t_parser_state *parse, t_rule *rule);
static uint64_t	reduce_filename(t_ctx *c, t_parser_state *parse, t_rule *rule);
static uint64_t	reduce_io_here(t_ctx *c, t_parser_state *parse, t_rule *rule);
static uint64_t	reduce_subshell(t_ctx *c, t_parser_state *parse, t_rule *rule);

static const t_rule	rules[RULE_COUNT] =
{
	[0] = {NULL, 0, SYM_EOF},
	[1] = {NULL, 2, SYM_ACCEPT},
	[2] = {reduce_program, 3, SYM_PROGRAM},
	[3] = {NULL, 1, SYM_PROGRAM},
	[4] = {reduce_complete_commands, 3, SYM_COMPLETE_COMMANDS},
	[5] = {NULL, 1, SYM_COMPLETE_COMMANDS},
	[6] = {NULL, 1, SYM_LIST},
	[7] = {reduce_list_and_if, 4, SYM_LIST},
	[8] = {reduce_list_or_if, 4, SYM_LIST},
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
	[42] = {NULL, 1, SYM_SEPARATOR},
	[43] = {NULL, 2, SYM_SEPARATOR},
	[44] = {NULL, 1, SYM_LINEBREAK},
	[45] = {NULL, 0, SYM_LINEBREAK},
};

static t_rule	get_rule(int32_t action)
{
	return (rules[action]);
}

/* -------- push / pop ------------------------------------------------------- */
static void	push_term(t_ctx *c, t_parser_state *parse)
{
	t_symbol	*symbol;
	t_arena		*stack;
	t_arena		*tokens;
	t_token		*token;

	stack = &(c->arena[AT_STACK]);
	tokens = &(c->arena[AT_TOKENS]);
	parse->stack_idx = get_idx_from_offset(stack,
			arena_alloc(stack, sizeof(t_symbol), _Alignof(t_symbol)));
	symbol = get_ptr_from_idx(stack, parse->stack_idx);
	token = get_ptr_from_idx(tokens, parse->token_idx);
	symbol->token_idx = parse->token_idx;
	symbol->type = classify_token(c, token);
	symbol->entry_state = parse->state;
	symbol->node_idx = 0;
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
	symbol->token_idx = token_idx;
	symbol->type = type;
	symbol->entry_state = parse->state;
	symbol->node_idx = node_idx;
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

/* -------- handlers --------------------------------------------------------- */
static uint64_t	reduce_program(t_ctx *c, t_parser_state *parse, t_rule *rule)
{
	(void)c;
	(void)parse;
	(void)rule;
	return (stack_at(c, parse, rule, 1)->node_idx);
}

static uint64_t	reduce_complete_commands(t_ctx *c, t_parser_state *parse,
		t_rule *rule)
{
	t_node		*node;
	uint64_t	head;
	uint64_t	new;

	head = stack_at(c, parse, rule, 0)->node_idx;
	new = stack_at(c, parse, rule, 2)->node_idx;
	if (!head)
		return (new);
	node = node_at(c, head);
	while (node->data.pipeline.next_idx)
		node = node_at(c, node->data.pipeline.next_idx);
	node->data.pipeline.next_idx = new;
	return (head);
}

static uint64_t	reduce_list_and_if(t_ctx *c, t_parser_state *parse,
		t_rule *rule)
{
	t_node		*node;

	node = node_at(c, stack_at(c, parse, rule, 3)->node_idx);
	node->flags |= FLAG_AND_IF;
	return (stack_at(c, parse, rule, 0)->node_idx);
}

static uint64_t	reduce_list_or_if(t_ctx *c, t_parser_state *parse,
		t_rule *rule)
{
	t_node		*node;

	node = node_at(c, stack_at(c, parse, rule, 3)->node_idx);
	node->flags |= FLAG_OR_IF;
	return (stack_at(c, parse, rule, 0)->node_idx);
}

static uint64_t	reduce_pipeline_create(t_ctx *c, t_parser_state *parse,
		t_rule *rule)
{
	uint64_t	p;

	p = node_alloc(c, NODE_PIPELINE);
	node_at(c, p)->data.pipeline.command_head_idx
		= stack_at(c, parse, rule, 0)->node_idx;
	return (p);
}

static uint64_t	reduce_pipeline_append(t_ctx *c, t_parser_state *parse,
		t_rule *rule)
{
	t_node		*node;
	uint64_t	p;
	uint64_t	head;

	p = node_alloc(c, NODE_PIPELINE);
	node_at(c, p)->data.pipeline.command_head_idx
		= stack_at(c, parse, rule, 3)->node_idx;
	head = stack_at(c, parse, rule, 0)->node_idx;
	if (!head)
		return (p);
	node = node_at(c, head);
	while (node->data.pipeline.next_idx)
		node = node_at(c, node->data.pipeline.next_idx);
	node->data.pipeline.next_idx = p;
	return (head);
}

static void	simple_command_positions(t_ctx *c, t_parser_state *parse,
		t_rule *rule, uint64_t *name, uint64_t *prefix, uint64_t *suffix)
{
	*name = 0;
	*prefix = 0;
	*suffix = 0;
	if (rule->rhs_len == 3)
	{
		*prefix = stack_at(c, parse, rule, 0)->node_idx;
		*name = stack_at(c, parse, rule, 1)->node_idx;
		*suffix = stack_at(c, parse, rule, 2)->node_idx;
	}
	else if (rule->rhs_len == 2)
	{
		if (stack_at(c, parse, rule, 1)->type == SYM_CMD_SUFFIX)
		{
			*name = stack_at(c, parse, rule, 0)->node_idx;
			*suffix = stack_at(c, parse, rule, 1)->node_idx;
		}
		else
		{
			*prefix = stack_at(c, parse, rule, 0)->node_idx;
			*name = stack_at(c, parse, rule, 1)->node_idx;
		}
	}
	else if (stack_at(c, parse, rule, 0)->type == SYM_CMD_PREFIX)
		*prefix = stack_at(c, parse, rule, 0)->node_idx;
	else
		*name = stack_at(c, parse, rule, 0)->node_idx;
}

static uint64_t	reduce_simple_command(t_ctx *c, t_parser_state *parse,
		t_rule *rule)
{
	uint64_t	name;
	uint64_t	prefix;
	uint64_t	suffix;
	uint64_t	args;
	uint64_t	redirs;
	uint64_t	cmd;

	simple_command_positions(c, parse, rule, &name, &prefix, &suffix);
	args = 0;
	redirs = 0;
	split_suffix(c, suffix, &args, &redirs);
	if (name)
		args = chain_append(c, name, args);
	cmd = node_alloc(c, NODE_COMMAND);
	node_at(c, cmd)->data.command.arg_head_idx = args;
	node_at(c, cmd)->data.command.redir_head_idx
		= chain_append(c, prefix, redirs);
	return (cmd);
}

static uint64_t	reduce_cmd_name(t_ctx *c, t_parser_state *parse, t_rule *rule)
{
	return (arg_leaf(c, parse, rule));
}

static uint64_t	reduce_cmd_word(t_ctx *c, t_parser_state *parse, t_rule *rule)
{
	return (arg_leaf(c, parse, rule));
}

static uint64_t	reduce_cmd_arg(t_ctx *c, t_parser_state *parse, t_rule *rule)
{
	return (arg_leaf(c, parse, rule));
}

static uint64_t	reduce_cmd_arg_append(t_ctx *c, t_parser_state *parse,
		t_rule *rule)
{
	return (chain_append(c, stack_at(c, parse, rule, 0)->node_idx,
			arg_leaf(c, parse, rule)));
}

static uint64_t	reduce_chain_append(t_ctx *c, t_parser_state *parse,
		t_rule *rule)
{
	return (chain_append(c, stack_at(c, parse, rule, 0)->node_idx,
			stack_at(c, parse, rule, 1)->node_idx));
}

static uint64_t	reduce_io_file_LESS(t_ctx *c, t_parser_state *parse,
		t_rule *rule)
{
	t_node		*node;

	node = node_at(c, stack_at(c, parse, rule, 1)->node_idx);
	node->flags |= REDIR_IN;
	return (stack_at(c, parse, rule, 1)->node_idx);
}

static uint64_t	reduce_io_file_GREAT(t_ctx *c, t_parser_state *parse,
		t_rule *rule)
{
	t_node		*node;

	node = node_at(c, stack_at(c, parse, rule, 1)->node_idx);
	node->flags |= REDIR_OUT;
	return (stack_at(c, parse, rule, 1)->node_idx);
}

static uint64_t	reduce_io_file_DGREAT(t_ctx *c, t_parser_state *parse,
		t_rule *rule)
{
	t_node		*node;

	node = node_at(c, stack_at(c, parse, rule, 1)->node_idx);
	node->flags |= REDIR_APPEND;
	return (stack_at(c, parse, rule, 1)->node_idx);
}

static uint64_t	reduce_filename(t_ctx *c, t_parser_state *parse, t_rule *rule)
{
	return (redir_leaf(c, parse, rule));
}

static uint64_t	reduce_io_here(t_ctx *c, t_parser_state *parse, t_rule *rule)
{
	t_symbol	*symbol;
	t_token		*token;
	t_arena		*strings;
	uint64_t	idx;
	uint8_t		flags;

	strings = &c->arena[AT_STRING];
	symbol = stack_at(c, parse, rule, 1);
	token = get_ptr_from_idx(&c->arena[AT_TOKENS], symbol->token_idx);
	parse->here.delim.pos = token->offset;
	parse->here.delim.len = ft_strlen(get_ptr_from_offset(strings, token->offset));
	flags = REDIR_HERE;
	if (token->flags & TKN_HAS_QUOTES)
	{
		parse->here.delim.pos += 1;
		parse->here.delim.len -= 2;
		flags |= REDIR_HAS_QUOTES;
	}
	parse->here.body.pos = 0;
	parse->here.body.len = 0;
	parse->flags |= PARSE_SAVE_TOKENS;
	idx = node_alloc(c, NODE_REDIR);
	node_at(c, idx)->flags = flags;
	return (idx);
}

static uint64_t	reduce_subshell(t_ctx *c, t_parser_state *parse, t_rule *rule)
{
	t_node		*node;

	node = node_at(c, stack_at(c, parse, rule, 1)->node_idx);
	node->flags |= FLAG_SUBSHELL;
	return (stack_at(c, parse, rule, 1)->node_idx);
}

/* -------- LALR driver ------------------------------------------------------ */
static t_lalr_action	reduce_or_error(t_ctx *c, t_parser_state *parse,
		int32_t action)
{
	if (!action)
		return (LALR_ERROR);
	return (reduce(c, parse, action));
}

static t_lalr_action	reduce(t_ctx *c, t_parser_state *parse, int32_t action)
{
	t_rule		rule;
	uint64_t	node_idx;
	uint64_t	token_idx;
	int32_t		lhs;
	int32_t		index;

	if (action == 1)						/* $accept: program $end */
		return (LALR_ACCEPT);
	rule = get_rule(action);
	if (rule.rhs_len)
	{
		if (rule.handler)
			node_idx = rule.handler(c, parse, &rule);
		else
			node_idx = stack_at(c, parse, &rule, 0)->node_idx;
		token_idx = stack_at(c, parse, &rule, rule.rhs_len - 1)->token_idx;
	}
	else
	{
		node_idx = 0;
		token_idx = parse->token_idx;
	}
	pop(c, parse, rule.rhs_len);
	lhs = rule.lhs_type - NTERM_OFFSET;
	index = yypgoto[lhs] + parse->state;
	if (0 <= index && index <= YYLAST && yycheck[index] == parse->state)
		parse->state = yytable[index];
	else
		parse->state = yydefgoto[lhs];
	push_nonterm(c, parse, rule.lhs_type, node_idx, token_idx);
	if (parse->state == YYFINAL)
		return (LALR_ACCEPT);
	return (LALR_REDUCE);
}

static t_lalr_action	shift(t_ctx *c, t_parser_state *parse, int32_t action)
{
	parse->state = action;
	if (parse->state == YYFINAL)
		return (LALR_ACCEPT);
	push_term(c, parse);
	return (LALR_SHIFT);
}

t_lalr_action	shift_reduce(t_ctx *c, t_parser_state *parse,
		t_lexer_state *lex)
{
	t_arena		*tokens;
	int32_t		action;
	int32_t		index;
	int32_t		lookahead;

	(void)lex;
	tokens = &(c->arena[AT_TOKENS]);
	index = yypact[parse->state];
	if (index == YYPACT_NINF)
		return (reduce_or_error(c, parse, yydefact[parse->state]));
	if (parse->flags & PARSE_LOOKAHEAD_EOF)
		lookahead = SYM_EOF;
	else
		lookahead = classify_token(c, get_ptr_from_idx(tokens, parse->token_idx));
	index += lookahead;
	if (index < 0 || index > YYLAST || yycheck[index] != lookahead)
		return (reduce_or_error(c, parse, yydefact[parse->state]));
	action = yytable[index];
	if (action <= 0)
		return (reduce_or_error(c, parse, -action));
	return (shift(c, parse, action));
}
