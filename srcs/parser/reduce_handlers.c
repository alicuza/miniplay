t_lalr_action	reduce(t_ctx *c, t_parser_state *parse, int32_t action);
uint64_t		reduce_program(t_ctx *c, t_parser_state *parse, t_rule *rule);
uint64_t		reduce_complete_commands(t_ctx *c, t_parser_state *parse, t_rule *rule);
uint64_t		reduce_list_and_if(t_ctx *c, t_parser_state *parse, t_rule *rule);
uint64_t		reduce_list_or_if(t_ctx *c, t_parser_state *parse, t_rule *rule);
uint64_t		reduce_pipeline_create(t_ctx *c, t_parser_state *parse, t_rule *rule);
uint64_t		reduce_pipeline_append(t_ctx *c, t_parser_state *parse, t_rule *rule);
uint64_t		reduce_simple_command(t_ctx *c, t_parser_state *parse, t_rule *rule);
uint64_t		reduce_cmd_name(t_ctx *c, t_parser_state *parse, t_rule *rule);
uint64_t		reduce_cmd_word(t_ctx *c, t_parser_state *parse, t_rule *rule);
uint64_t		reduce_cmd_arg(t_ctx *c, t_parser_state *parse, t_rule *rule);
uint64_t		reduce_cmd_arg_append(t_ctx *c, t_parser_state *parse, t_rule *rule);
uint64_t		reduce_chain_append(t_ctx *c, t_parser_state *parse, t_rule *rule);
uint64_t		reduce_io_file_LESS(t_ctx *c, t_parser_state *parse, t_rule *rule);
uint64_t		reduce_io_file_GREAT(t_ctx *c, t_parser_state *parse, t_rule *rule);
uint64_t		reduce_io_file_DGREAT(t_ctx *c, t_parser_state *parse, t_rule *rule);
uint64_t		reduce_filename(t_ctx *c, t_parser_state *parse, t_rule *rule);
uint64_t		reduce_io_here(t_ctx *c, t_parser_state *parse, t_rule *rule);
uint64_t		reduce_subshell(t_ctx *c, t_parser_state *parse, t_rule *rule);

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

