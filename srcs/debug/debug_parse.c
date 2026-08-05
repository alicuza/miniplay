/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_parse.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sancuta <sancuta@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 11:08:25 by sancuta           #+#    #+#             */
/*   Updated: 2026/08/04 17:49:49 by sancuta          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* leaf non-terminals wrap a single WORD and keep its content */
static bool	symbol_has_content(t_symbol_type type)
{
	if (type == SYM_CMD_NAME || type == SYM_CMD_WORD
		|| type == SYM_FILENAME || type == SYM_HERE_END
		|| type == SYM_WORD || type == SYM_NEWLINE
		|| type == SYM_PIPE || type == SYM_LESS || type == SYM_GREAT
		|| type == SYM_DLESS || type == SYM_DGREAT
		|| type == SYM_AND_IF || type == SYM_OR_IF
		|| type == SYM_OPAR || type == SYM_CPAR)
		return (true);
	return (false);
}

static void	build_symbol_desc(t_ctx *c, t_symbol *symbol, uint64_t idx,
		char *buf, size_t size)
{
	t_arena	*tokens;
	t_token	*token;
	t_arena	*input;
	size_t	pos;

	tokens = &c->arena[AT_TOKENS];
	input = &c->arena[AT_STRING];
	token = get_ptr_from_idx(tokens, symbol->token_idx);
	pos = (size_t)snprintf(buf, size, "%lu %s(", idx,
		get_symbol_type_name(symbol->type));
	if (symbol->type == SYM_LINEBREAK && input->buf[token->offset] != '\n')
		pos += (size_t)snprintf(buf + pos, size - pos, "epsilon");
	else if (symbol_has_content(symbol->type))
		pos += escape_into_buf(buf + pos, size - pos,
			input->buf + token->offset, ft_strlen(input->buf + token->offset));
	else
		pos += (size_t)snprintf(buf + pos, size - pos, "node %lu",
			symbol->node_idx);
	snprintf(buf + pos, size - pos, ")");
}

void	print_symbol(FILE *out, t_ctx *c, t_symbol *symbol, uint64_t idx)
{
	t_arena	*tokens;
	t_token	*token;
	char	desc[256];

	tokens = &c->arena[AT_TOKENS];
	token = get_ptr_from_idx(tokens, symbol->token_idx);
	build_symbol_desc(c, symbol, idx, desc, sizeof(desc));
	fprintf(out, "%s { node_idx = %lu entry_state = %u flags = ", desc,
		symbol->node_idx, symbol->entry_state);
	print_flags(out, token->flags);
	fprintf(out, " }\n");
}

static void	build_lookahead_desc(t_ctx *c, t_parser_state *parse, char *buf,
		size_t size);

void	print_stack(FILE *out, t_ctx *c, t_parser_state *parse)
{
	uint64_t	phys;
	t_symbol	*symbol;
	t_arena		*stack;
	char		lookahead[96];

	stack = &c->arena[AT_STACK];
	fprintf(out, "\n--- stack ---\n(state %d)\n", parse->state);
	fprintf(out, "--- top -----\n");
	phys = parse->stack_idx;
	while (phys)
	{
		symbol = get_ptr_from_idx(stack, phys);
		print_symbol(out, c, symbol, phys);
		--phys;
	}
	fprintf(out, "--- bottom ----\n");
	if (c->dbg.awaiting)
		fprintf(out, "lookahead: " DBG_LOOKAHEAD_PENDING "\n");
	else if (parse->flags & PARSE_LOOKAHEAD_EOF)
		fprintf(out, "lookahead: " DBG_LOOKAHEAD_EOF "\n");
	else
	{
		build_lookahead_desc(c, parse, lookahead, sizeof(lookahead));
		fprintf(out, "lookahead: %s\n", lookahead);
	}
	fprintf(out, "action: %s\n", c->dbg.last_action);
}

static void	build_stack_desc(t_ctx *c, t_parser_state *parse, char *buf,
		size_t size)
{
	t_symbol	*symbol;
	char		desc[256];
	size_t		pos;
	uint64_t	phys;

	pos = 0;
	phys = 1;
	while (phys <= parse->stack_idx)
	{
		symbol = get_ptr_from_idx(&c->arena[AT_STACK], phys);
		build_symbol_desc(c, symbol, phys, desc, sizeof(desc));
		pos += ft_strlcpy(buf + pos, desc, size - pos);
		pos += ft_strlcpy(buf + pos, " ", size - pos);
		++phys;
	}
}

static void	build_lookahead_desc(t_ctx *c, t_parser_state *parse, char *buf,
		size_t size)
{
	t_token	*token;
	char	*body;
	size_t	pos;

	if (parse->flags & PARSE_LOOKAHEAD_EOF)
	{
		ft_strlcpy(buf, DBG_LOOKAHEAD_EOF, size);
		return ;
	}
	token = get_ptr_from_idx(&c->arena[AT_TOKENS], parse->token_idx);
	body = get_ptr_from_offset(&c->arena[AT_STRING], token->offset);
	pos = (size_t)snprintf(buf, size, "%s(",
		get_symbol_type_name(classify_token(c, token)));
	pos += escape_into_buf(buf + pos, size - pos, body, ft_strlen(body));
	snprintf(buf + pos, size - pos, ")");
}

#define STACK_COL_WIDTH		180
#define LOOKAHEAD_COL_WIDTH	28

void	print_parse_table(FILE *out, t_ctx *c, t_parser_state *parse,
		const char *action)
{
	char	stack_desc[4096];
	char	lookahead[96];

	build_stack_desc(c, parse, stack_desc, sizeof(stack_desc));
	build_lookahead_desc(c, parse, lookahead, sizeof(lookahead));
	fprintf(out, "[bottom] %-*s [top] | %-*s | %s\n", STACK_COL_WIDTH,
		stack_desc, LOOKAHEAD_COL_WIDTH, lookahead, action);
}

void	print_trace_line(FILE *out, t_ctx *c, t_parser_state *parse,
		const char *action)
{
	char	stack_desc[4096];
	char	lookahead[96];

	build_stack_desc(c, parse, stack_desc, sizeof(stack_desc));
	build_lookahead_desc(c, parse, lookahead, sizeof(lookahead));
	fprintf(out, "[bottom] %s[top] | %s | %s\n",
		stack_desc, lookahead, action);
}

void	print_tokens(FILE *out, t_ctx *c)
{
	t_arena	*tokens;
	t_token	*token;
	uint64_t	count;
	uint64_t	i;

	tokens = &c->arena[AT_TOKENS];
	if (tokens->cap == 0)
		return ;
	count = (tokens->offset - tokens->stride) / tokens->stride;
	if (count == 0)
		return ;
	fprintf(out, "\n--- tokens ---\n");
	i = 1;
	while (i <= count)
	{
		token = get_ptr_from_idx(tokens, i);
		print_token_line(out, c, token);
		++i;
	}
}

static void	print_node_flags(FILE *out, uint8_t flags)
{
	uint32_t	bit;

	bit = 1;
	while (bit && !(flags & bit))
		bit <<= 1;
	while (bit)
	{
		if (flags & bit)
		{
			fprintf(out, " %s", get_node_flag_name(bit));
			flags ^= bit;
		}
		bit <<= 1;
	}
}

void	print_node_line(FILE *out, t_ctx *c, t_node *node, uint64_t idx)
{
	t_arena	*strings;

	strings = &c->arena[AT_STRING];
	fprintf(out, "[id %lu] %s", idx, get_node_type_name(node->type));
	if (node->type == NODE_PIPELINE)
	{
		fprintf(out, " [next %lu] [command_head %lu]",
			node->data.pipeline.next_idx,
			node->data.pipeline.command_head_idx);
	}
	else if (node->type == NODE_COMMAND)
	{
		fprintf(out, " [next %lu] [arg_head %lu] [redir_head %lu]",
			node->data.command.next,
			node->data.command.arg_head_idx,
			node->data.command.redir_head_idx);
	}
	else if (node->type == NODE_ARG)
	{
		fprintf(out, "(");
		if (node->data.arg.arena_offset)
			print_escaped_str(out, strings->buf + node->data.arg.arena_offset);
		fprintf(out, ") [next %lu]", node->data.arg.next);
	}
	else if (node->type == NODE_REDIR)
	{
		fprintf(out, "(");
		if (node->flags & REDIR_HERE)
		{
			fprintf(out, "<< ");
			if (node->data.redir.arena_offset)
				print_escaped_str(out, strings->buf + node->data.redir.arena_offset);
		}
		else if (node->flags & REDIR_OUT)
		{
			fprintf(out, "> ");
			if (node->data.redir.arena_offset)
				print_escaped_str(out, strings->buf + node->data.redir.arena_offset);
		}
		else if (node->flags & REDIR_APPEND)
		{
			fprintf(out, ">> ");
			if (node->data.redir.arena_offset)
				print_escaped_str(out, strings->buf + node->data.redir.arena_offset);
		}
		else if (node->flags & REDIR_IN)
		{
			fprintf(out, "< ");
			if (node->data.redir.arena_offset)
				print_escaped_str(out, strings->buf + node->data.redir.arena_offset);
		}
		fprintf(out, ") [next %lu]", node->data.redir.next);
	}
	print_node_flags(out, node->flags);
	fprintf(out, "\n");
}

void	print_nodes(FILE *out, t_ctx *c)
{
	t_arena	*commands;
	t_node	*node;
	uint64_t	count;
	uint64_t	i;

	commands = &c->arena[AT_COMMAND];
	if (commands->cap == 0)
		return ;
	count = (commands->offset - commands->stride) / commands->stride;
	if (count == 0)
		return ;
	fprintf(out, "\n--- nodes ---\n");
	i = 1;
	while (i <= count)
	{
		node = get_ptr_from_idx(commands, i);
		print_node_line(out, c, node, i);
		++i;
	}
}
