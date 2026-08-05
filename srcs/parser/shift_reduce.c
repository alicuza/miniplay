#include "minishell.h"

#define YYPACT_NINF (-44)
#define YYFINAL      5
#define YYLAST       67
#define NTERM_OFFSET 14

/* -------- LALR tables, regenerated from shni_grammar_reduced.y ----------- */
static inline int32_t	get_yypact(uint64_t idx)
{
	static const int32_t	yypact[] = {
		  8,  -44,   18,   21,   33,  -44,  -44,  -44,   14,   14,
		 30,   14,    8,    8,   17,   16,  -44,   58,  -44,   40,
		 47,  -44,  -44,  -44,  -44,  -44,  -44,  -44,  -44,  -44,
		 24,   33,    7,  -44,    8,    8,    8,   58,  -44,  -44,
		 54,  -44,  -44,   40,  -44,  -44,   17,    8,   17,   33,
		 33,   33,  -44,  -44,  -44,   54,    7,   16,   16,  -44,
		 17
	};
	return (yypact[idx]);
}

static inline int32_t	get_yydefact(uint64_t idx)
{
	static const int32_t	yydefact[] = {
		 45,   42,    0,   44,    3,    1,   43,   24,    0,    0,
		  0,    0,   45,   45,    5,    6,    9,   12,   11,   23,
		 21,   26,   34,   35,   39,   36,   37,   41,   40,   38,
		  0,    0,   44,    2,   45,   45,   45,   13,   32,   30,
		 22,   28,   25,   20,   27,   14,   18,   15,    4,    0,
		  0,    0,   33,   31,   29,   19,   16,    7,    8,   10,
		 17
	};
	return (yydefact[idx]);
}

static inline int32_t	get_yypgoto(uint64_t idx)
{
	static const int32_t	yypgoto[] = {
		-44,  -44,  -44,  -27,  -43,   -7,  -44,  -44,  -44,  -44,
		-44,  -44,  -44,   -9,  -44,  -17,  -44,   13,  -44,  -44,
		-12,   -4
	};
	return (yypgoto[idx]);
}

static inline int32_t	get_yydefgoto(uint64_t idx)
{
	static const int32_t	yydefgoto[] = {
		  0,    2,   13,   14,   15,   16,   17,   30,   47,   18,
		 19,   43,   20,   40,   37,   21,   22,   25,   23,   28,
		  3,    4
	};
	return (yydefgoto[idx]);
}

static inline int32_t	get_yytable(uint64_t idx)
{
	static const int32_t	yytable[] = {
		 38,   32,   41,   44,   46,   48,   57,   58,   31,   33,
		  7,    6,    1,    8,    9,   10,   11,   24,    5,   12,
		 52,   36,   26,   54,   29,    6,   41,   34,   35,   60,
		 49,   50,   51,   27,   55,   56,    7,   45,   54,    8,
		  9,   10,   11,   39,   59,   12,    8,    9,   10,   11,
		 42,    0,    0,    8,    9,   10,   11,   53,    0,    0,
		  8,    9,   10,   11,    8,    9,   10,   11
	};
	return (yytable[idx]);
}

static inline int32_t	get_yycheck(uint64_t idx)
{
	static const int32_t	yycheck[] = {
		 17,   13,   19,   20,   31,   32,   49,   50,   12,   13,
		  3,    4,    4,    6,    7,    8,    9,    3,    0,   12,
		 37,    5,    9,   40,   11,    4,   43,   10,   11,   56,
		 34,   35,   36,    3,   43,   47,    3,   13,   55,    6,
		  7,    8,    9,    3,   51,   12,    6,    7,    8,    9,
		  3,   -1,   -1,    6,    7,    8,    9,    3,   -1,   -1,
		  6,    7,    8,    9,    6,    7,    8,    9
	};
	return (yycheck[idx]);
}

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

static t_symbol	*stack_at(t_ctx *c, t_parser_state *parse, t_rule *rule,
		uint32_t i)
{
	t_arena	*stack;

	(void)parse;
	stack = &c->arena[AT_STACK];
	return (get_ptr_from_offset(stack,
			stack->offset - (rule->rhs_len - i) * stack->stride));
}

static uint64_t	node_next(t_node *node)
{
	if (node->type == NODE_COMMAND)
		return (node->data.command.next);
	if (node->type == NODE_PIPELINE)
		return (node->data.pipeline.next_idx);
	if (node->type == NODE_REDIR)
		return (node->data.redir.next);
	return (node->data.arg.next);
}

static void	node_link(t_node *node, uint64_t next)
{
	if (node->type == NODE_COMMAND)
		node->data.command.next = next;
	else if (node->type == NODE_PIPELINE)
		node->data.pipeline.next_idx = next;
	else if (node->type == NODE_REDIR)
		node->data.redir.next = next;
	else
		node->data.arg.next = next;
}

static uint64_t	reduce_chain_append(t_ctx *c, uint64_t head, uint64_t new)
{
	t_node	*node;
	uint64_t	cur;

	if (!new)
		return (head);
	if (!head)
		return (new);
	cur = head;
	while (cur)
	{
		node = node_at(c, cur);
		cur = node_next(node);
	}
	node_link(node, new);
	return (head);
}

static uint64_t	reduce_leaf(t_ctx *c, t_parser_state *parse, t_rule *rule,
		t_node_type type)
{
	t_symbol	*symbol;
	t_token		*token;
	uint64_t	idx;

	symbol = stack_at(c, parse, rule, rule->rhs_len - 1);
	token = get_ptr_from_idx(&c->arena[AT_TOKENS], symbol->token_idx);
	idx = node_alloc(c, type);
	if (type == NODE_REDIR)
		node_at(c, idx)->data.redir.arena_offset = token->offset;
	else
		node_at(c, idx)->data.arg.arena_offset = token->offset;
	return (idx);
}

static uint64_t	reduce_arg_leaf(t_ctx *c, t_parser_state *parse, t_rule *rule)
{
	return (reduce_leaf(c, parse, rule, NODE_ARG));
}

static uint64_t	reduce_redir_leaf(t_ctx *c, t_parser_state *parse, t_rule *rule)
{
	return (reduce_leaf(c, parse, rule, NODE_REDIR));
}

/* -------- reduction handlers (index == bison rule number) ------------------ */
static t_lalr_action	reduce(t_ctx *c, t_parser_state *parse, int32_t action);
static uint64_t	reduce_program(t_ctx *c, t_parser_state *parse, t_rule *rule);
static uint64_t	reduce_pipeline_create(t_ctx *c, t_parser_state *parse, t_rule *rule);
static uint64_t	reduce_pipeline_append(t_ctx *c, t_parser_state *parse, t_rule *rule);
static uint64_t	reduce_list_append(t_ctx *c, t_parser_state *parse, t_rule *rule);
static uint64_t	reduce_list_conditional(t_ctx *c, t_parser_state *parse,
		t_rule *rule);
static uint64_t	reduce_simple_command(t_ctx *c, t_parser_state *parse, t_rule *rule);
static uint64_t	reduce_simple_command_bare(t_ctx *c, t_parser_state *parse,
		t_rule *rule);
static uint64_t	reduce_cmd_suffix(t_ctx *c, t_parser_state *parse, t_rule *rule);
static uint64_t	reduce_redir_append(t_ctx *c, t_parser_state *parse, t_rule *rule);
static uint64_t	reduce_io_file(t_ctx *c, t_parser_state *parse, t_rule *rule);
static uint64_t	reduce_io_here(t_ctx *c, t_parser_state *parse, t_rule *rule);
static uint64_t	reduce_command_redirects(t_ctx *c, t_parser_state *parse,
		t_rule *rule);
static uint64_t	reduce_compound_list(t_ctx *c, t_parser_state *parse,
		t_rule *rule);
static uint64_t	reduce_subshell(t_ctx *c, t_parser_state *parse, t_rule *rule);

static t_rule	rule_dispatch_first(int32_t action)
{
	static const t_rule	rule[16] = {
		{NULL, 0, SYM_EOF},
		{NULL, 2, SYM_ACCEPT},
		{reduce_program, 3, SYM_PROGRAM},
		{NULL, 1, SYM_PROGRAM},
		{reduce_list_append, 3, SYM_COMPLETE_COMMANDS},
		{NULL, 1, SYM_COMPLETE_COMMANDS},
		{NULL, 1, SYM_LIST},
		{reduce_list_conditional, 4, SYM_LIST},
		{reduce_list_conditional, 4, SYM_LIST},
		{reduce_pipeline_create, 1, SYM_PIPELINE},
		{reduce_pipeline_append, 4, SYM_PIPELINE},
		{NULL, 1, SYM_COMMAND},
		{NULL, 1, SYM_COMMAND},
		{reduce_command_redirects, 2, SYM_COMMAND},
		{reduce_subshell, 3, SYM_SUBSHELL},
		{reduce_compound_list, 2, SYM_COMPOUND_LIST}
	};
	return (rule[action]);
}

static t_rule	rule_dispatch_second(int32_t action)
{
	static const t_rule	rule[16] = {
		{reduce_compound_list, 3, SYM_COMPOUND_LIST},
		{reduce_list_append, 3, SYM_TERM},
		{NULL, 1, SYM_TERM},
		{reduce_simple_command, 3, SYM_SIMPLE_COMMAND},
		{reduce_simple_command_bare, 2, SYM_SIMPLE_COMMAND},
		{reduce_simple_command_bare, 1, SYM_SIMPLE_COMMAND},
		{reduce_simple_command, 2, SYM_SIMPLE_COMMAND},
		{reduce_simple_command_bare, 1, SYM_SIMPLE_COMMAND},
		{reduce_arg_leaf, 1, SYM_CMD_NAME},
		{reduce_arg_leaf, 1, SYM_CMD_WORD},
		{NULL, 1, SYM_CMD_PREFIX},
		{reduce_redir_append, 2, SYM_CMD_PREFIX},
		{reduce_cmd_suffix, 1, SYM_CMD_SUFFIX},
		{reduce_cmd_suffix, 2, SYM_CMD_SUFFIX},
		{reduce_cmd_suffix, 1, SYM_CMD_SUFFIX},
		{reduce_cmd_suffix, 2, SYM_CMD_SUFFIX}
	};
	return (rule[action - 16]);
}

static t_rule	rule_dispatch_third(int32_t action)
{
	static const t_rule	rule[14] = {
		{NULL, 1, SYM_REDIRECT_LIST},
		{reduce_redir_append, 2, SYM_REDIRECT_LIST},
		{NULL, 1, SYM_IO_REDIRECT},
		{NULL, 1, SYM_IO_REDIRECT},
		{reduce_io_file, 2, SYM_IO_FILE},
		{reduce_io_file, 2, SYM_IO_FILE},
		{reduce_io_file, 2, SYM_IO_FILE},
		{reduce_redir_leaf, 1, SYM_FILENAME},
		{reduce_io_here, 2, SYM_IO_HERE},
		{NULL, 1, SYM_HERE_END},
		{NULL, 1, SYM_SEPARATOR},
		{NULL, 2, SYM_SEPARATOR},
		{NULL, 1, SYM_LINEBREAK},
		{NULL, 0, SYM_LINEBREAK}
	};
	return (rule[action - 32]);
}

static t_rule	get_rule(int32_t action)
{
	if (action < 16)
		return (rule_dispatch_first(action));
	if (action < 32)
		return (rule_dispatch_second(action));
	return (rule_dispatch_third(action));
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

static uint64_t	reduce_list_append(t_ctx *c, t_parser_state *parse,
		t_rule *rule)
{
	return (reduce_chain_append(c, stack_at(c, parse, rule, 0)->node_idx,
			stack_at(c, parse, rule, rule->rhs_len - 1)->node_idx));
}

static uint64_t	reduce_list_conditional(t_ctx *c, t_parser_state *parse,
		t_rule *rule)
{
	t_node		*node;

	node = node_at(c, stack_at(c, parse, rule, 3)->node_idx);
	if (stack_at(c, parse, rule, 1)->type == SYM_AND_IF)
		node->flags |= FLAG_AND_IF;
	else
		node->flags |= FLAG_OR_IF;
	return (reduce_list_append(c, parse, rule));
}

static uint64_t	reduce_pipeline_create(t_ctx *c, t_parser_state *parse,
		t_rule *rule)
{
	t_node		*node;
	uint64_t	p;

	p = node_alloc(c, NODE_PIPELINE);
	node = node_at(c, p);
	node->data.pipeline.command_head_idx = stack_at(c, parse, rule, 0)->node_idx;
	return (p);
}

static uint64_t	reduce_pipeline_append(t_ctx *c, t_parser_state *parse,
		t_rule *rule)
{
	t_node		*node;

	node = node_at(c, stack_at(c, parse, rule, 0)->node_idx);
	node->data.pipeline.command_head_idx = reduce_chain_append(c,
			node->data.pipeline.command_head_idx,
			stack_at(c, parse, rule, 3)->node_idx);
	return (stack_at(c, parse, rule, 0)->node_idx);
}

static uint64_t	reduce_simple_command(t_ctx *c, t_parser_state *parse,
		t_rule *rule)
{
	t_node		*cmd;
	uint64_t	cmd_idx;
	uint64_t	name;
	uint64_t	prefix;

	cmd_idx = stack_at(c, parse, rule, rule->rhs_len - 1)->node_idx;
	name = stack_at(c, parse, rule, rule->rhs_len - 2)->node_idx;
	prefix = 0;
	if (rule->rhs_len == 3)
		prefix = stack_at(c, parse, rule, 0)->node_idx;
	cmd = node_at(c, cmd_idx);
	cmd->data.command.arg_head_idx = reduce_chain_append(c, name,
			cmd->data.command.arg_head_idx);
	cmd->data.command.redir_head_idx = reduce_chain_append(c, prefix,
			cmd->data.command.redir_head_idx);
	return (cmd_idx);
}

static uint64_t	reduce_simple_command_bare(t_ctx *c, t_parser_state *parse,
		t_rule *rule)
{
	t_node		*cmd;
	uint64_t	cmd_idx;
	uint64_t	name;
	uint64_t	prefix;

	cmd_idx = node_alloc(c, NODE_COMMAND);
	cmd = node_at(c, cmd_idx);
	name = 0;
	prefix = 0;
	if (rule->rhs_len == 2)
	{
		prefix = stack_at(c, parse, rule, 0)->node_idx;
		name = stack_at(c, parse, rule, 1)->node_idx;
	}
	else if (stack_at(c, parse, rule, 0)->type == SYM_CMD_PREFIX)
		prefix = stack_at(c, parse, rule, 0)->node_idx;
	else
		name = stack_at(c, parse, rule, 0)->node_idx;
	cmd->data.command.arg_head_idx = name;
	cmd->data.command.redir_head_idx = prefix;
	return (cmd_idx);
}

static uint64_t	reduce_cmd_suffix(t_ctx *c, t_parser_state *parse,
		t_rule *rule)
{
	t_node		*cmd;
	t_symbol	*last;
	uint64_t	cmd_idx;

	if (rule->rhs_len == 1)
		cmd_idx = node_alloc(c, NODE_COMMAND);
	else
		cmd_idx = stack_at(c, parse, rule, 0)->node_idx;
	last = stack_at(c, parse, rule, rule->rhs_len - 1);
	cmd = node_at(c, cmd_idx);
	if (last->type == SYM_IO_REDIRECT)
		cmd->data.command.redir_head_idx = reduce_chain_append(c,
				cmd->data.command.redir_head_idx, last->node_idx);
	else
		cmd->data.command.arg_head_idx = reduce_chain_append(c,
				cmd->data.command.arg_head_idx,
				reduce_arg_leaf(c, parse, rule));
	return (cmd_idx);
}

static uint64_t	reduce_redir_append(t_ctx *c, t_parser_state *parse,
		t_rule *rule)
{
	return (reduce_chain_append(c, stack_at(c, parse, rule, 0)->node_idx,
			stack_at(c, parse, rule, 1)->node_idx));
}

static uint64_t	reduce_io_file(t_ctx *c, t_parser_state *parse, t_rule *rule)
{
	t_node		*node;
	uint64_t	idx;

	idx = stack_at(c, parse, rule, 1)->node_idx;
	node = node_at(c, idx);
	if (stack_at(c, parse, rule, 0)->type == SYM_LESS)
	{
		node->flags |= REDIR_IN;
		node->data.redir.fd = STDIN_FILENO;
	}
	else if (stack_at(c, parse, rule, 0)->type == SYM_GREAT)
	{
		node->flags |= REDIR_OUT;
		node->data.redir.fd = STDOUT_FILENO;
	}
	else
	{
		node->flags |= REDIR_APPEND;
		node->data.redir.fd = STDOUT_FILENO;
	}
	return (idx);
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
#ifdef DEBUG
	if (c->dbg.states & DBG_PARSER)
		fprintf(stderr, "--- parse --- heredoc: entering SAVE_TOKENS mode\n");
#endif
	parse->flags |= PARSE_SAVE_TOKENS;
	idx = node_alloc(c, NODE_REDIR);
	node_at(c, idx)->flags = flags;
	node_at(c, idx)->data.redir.arena_offset = token->offset;
	return (idx);
}

static uint64_t	reduce_command_redirects(t_ctx *c, t_parser_state *parse,
		t_rule *rule)
{
	t_node		*node;

	node = node_at(c, stack_at(c, parse, rule, 0)->node_idx);
	node->data.command.redir_head_idx = stack_at(c, parse, rule, 1)->node_idx;
	return (stack_at(c, parse, rule, 0)->node_idx);
}

static uint64_t	reduce_compound_list(t_ctx *c, t_parser_state *parse,
		t_rule *rule)
{
	return (stack_at(c, parse, rule, 1)->node_idx);
}

static uint64_t	reduce_subshell(t_ctx *c, t_parser_state *parse, t_rule *rule)
{
	t_node		*node;
	uint64_t	idx;

	idx = node_alloc(c, NODE_COMMAND);
	node = node_at(c, idx);
	node->flags |= FLAG_SUBSHELL;
	node->data.command.arg_head_idx = stack_at(c, parse, rule, 1)->node_idx;
	return (idx);
}

/* -------- LALR driver ------------------------------------------------------ */
static t_lalr_action	reduce_or_error(t_ctx *c, t_parser_state *parse,
		int32_t action)
{
	if (!action)
		return (LALR_ERROR);
	return (reduce(c, parse, action));
}

#ifdef DEBUG
static void	build_rule_desc(char *buf, size_t size, int32_t action,
		t_ctx *c, t_rule *rule, t_parser_state *parse)
{
	size_t		pos;
	uint64_t	rhs;

	pos = (size_t)snprintf(buf, size, "reduce %d (%s :=", action,
		get_symbol_type_name(rule->lhs_type));
	if (rule->rhs_len == 0)
		pos += (size_t)snprintf(buf + pos, size - pos, " (epsilon)");
	else
	{
		rhs = 0;
		while (rhs < rule->rhs_len)
		{
			pos += (size_t)snprintf(buf + pos, size - pos, " %s",
				get_symbol_type_name(
					stack_at(c, parse, rule, rhs)->type));
			++rhs;
		}
	}
	snprintf(buf + pos, size - pos, ")");
}

static void	print_trace_step(t_ctx *c, t_parser_state *parse,
		const char *label)
{
	ft_strlcpy(c->dbg.last_action, label, sizeof(c->dbg.last_action));
	if (c->dbg.states & DBG_PARSER)
		print_parse_table(stderr, c, parse, label);
	if (c->dbg.scope & SCOPE_TRACE)
		print_trace_line(stdout, c, parse, label);
}
#endif

static t_lalr_action	reduce(t_ctx *c, t_parser_state *parse, int32_t action)
{
	t_rule		rule;
	uint64_t	node_idx;
	uint64_t	token_idx;
	int32_t		lhs;
	int32_t		index;
#ifdef DEBUG
	uint32_t	rhs;
	t_symbol	*symbol;
	char		rule_desc[256];
#endif

	if (action == 1)						/* $accept: program $end */
	{
#ifdef DEBUG
		print_trace_step(c, parse, "accept");
#endif
		return (LALR_ACCEPT);
	}
	rule = get_rule(action);
#ifdef DEBUG
	build_rule_desc(rule_desc, sizeof(rule_desc), action, c, &rule, parse);
#endif
	if (rule.rhs_len)
	{
		if (rule.handler)
			node_idx = rule.handler(c, parse, &rule);
		else
			node_idx = stack_at(c, parse, &rule, 0)->node_idx;
		token_idx = stack_at(c, parse, &rule, rule.rhs_len - 1)->token_idx;
#ifdef DEBUG
		if (c->dbg.states & DBG_PARSER)
		{
			rhs = 0;
			while (rhs < rule.rhs_len)
			{
				symbol = stack_at(c, parse, &rule, rhs);
				if (symbol->node_idx)
					print_node_line(stderr, c, node_at(c, symbol->node_idx),
						symbol->node_idx);
				++rhs;
			}
		}
#endif
	}
	else
	{
		node_idx = 0;
		token_idx = parse->token_idx;
	}
	if (action == 4)					/* complete_commands: complete_commands separator list */
		parse->exec_idx = stack_at(c, parse, &rule, 2)->node_idx;
	else if (action == 5)				/* complete_commands: list */
		parse->exec_idx = stack_at(c, parse, &rule, 0)->node_idx;
	pop(c, parse, rule.rhs_len);
	lhs = rule.lhs_type - NTERM_OFFSET;
	index = get_yypgoto(lhs) + parse->state;
	if (0 <= index && index <= YYLAST && get_yycheck(index) == parse->state)
		parse->state = get_yytable(index);
	else
		parse->state = get_yydefgoto(lhs);
#ifdef DEBUG
	if (c->dbg.states & DBG_PARSER)
	{
		if (node_idx)
			print_node_line(stderr, c, node_at(c, node_idx), node_idx);
	}
#endif
	push_nonterm(c, parse, rule.lhs_type, node_idx, token_idx);
#ifdef DEBUG
	print_trace_step(c, parse, rule_desc);
#endif
	if (parse->state == YYFINAL)
	{
#ifdef DEBUG
		print_trace_step(c, parse, "accept");
#endif
		return (LALR_ACCEPT);
	}
	return (LALR_REDUCE);
}

static t_lalr_action	shift(t_ctx *c, t_parser_state *parse, int32_t action)
{
#ifdef DEBUG
	char	action_desc[64];
	char	*num;
#endif

	parse->state = action;
#ifdef DEBUG
	ft_strlcpy(action_desc, "shift -> state ", sizeof(action_desc));
	num = ft_itoa(parse->state); /* TODO: avoid malloc in debug path (valgrind noise) */
	if (num)
	{
		ft_strlcat(action_desc, num, sizeof(action_desc));
		free(num);
	}
	print_trace_step(c, parse, action_desc);
#endif
	if (parse->state == YYFINAL)
	{
#ifdef DEBUG
		print_trace_step(c, parse, "accept");
#endif
		return (LALR_ACCEPT);
	}
	push_term(c, parse);
	return (LALR_SHIFT);
}

t_lalr_action	shift_reduce(t_ctx *c, t_parser_state *parse,
		t_lexer_state *lex)
{
	t_arena		*tokens;
	t_token		*token;
	int32_t		action;
	int32_t		index;
	int32_t		lookahead;

	(void)lex;
	tokens = &(c->arena[AT_TOKENS]);
	index = get_yypact(parse->state);
	if (index == YYPACT_NINF)
		return (reduce_or_error(c, parse, get_yydefact(parse->state)));
	if (parse->flags & PARSE_LOOKAHEAD_EOF)
		lookahead = SYM_EOF;
	else
	{
		token = get_ptr_from_idx(tokens, parse->token_idx);
		lookahead = classify_token(c, token);
	}
	index += lookahead;
	if (index < 0 || index > YYLAST || get_yycheck(index) != lookahead)
		return (reduce_or_error(c, parse, get_yydefact(parse->state)));
	action = get_yytable(index);
	if (action <= 0)
		return (reduce_or_error(c, parse, -action));
	return (shift(c, parse, action));
}
