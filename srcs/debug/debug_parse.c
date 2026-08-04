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

void	print_symbol(t_ctx *c, t_symbol *symbol, uint64_t idx)
{
	t_arena	*tokens;
	t_token	*token;
	t_arena	*input;

	tokens = &c->arena[AT_TOKENS];
	input = &c->arena[AT_STRING];
	token = get_ptr_from_idx(tokens, symbol->token_idx);
	fprintf(stderr, "%lu %s(", idx, get_symbol_type_name(symbol->type));
	if (symbol->type == SYM_LINEBREAK && input->buf[token->offset] != '\n')
		fprintf(stderr, "epsilon");
	else
		print_escaped_str(stderr, input->buf + token->offset);
	fprintf(stderr, ") { node_idx = %lu flags = ", symbol->node_idx);
	print_flags(stderr, token->flags);
	fprintf(stderr, " }\n");
}

void	print_stack(t_ctx *c, t_parser_state *parse)
{
	uint64_t	phys;
	t_symbol	*symbol;
	t_arena		*stack;

	stack = &c->arena[AT_STACK];
	fprintf(stderr, "\n--- stack ---  (state %d)\n", parse->state);
	fprintf(stderr, "--- state stack: ");
	phys = 1;
	while (phys <= parse->stack_idx)
	{
		symbol = get_ptr_from_idx(stack, phys);
		fprintf(stderr, "%d ", symbol->entry_state);
		++phys;
	}
	fprintf(stderr, "---\n");
	fprintf(stderr, "--- top -----\n");
	phys = parse->stack_idx;
	while (phys)
	{
		symbol = get_ptr_from_idx(stack, phys);
		print_symbol(c, symbol, phys);
		--phys;
	}
	fprintf(stderr, "--- bottom ----\n");
}

void	print_symbol_line(FILE *out, t_ctx *c, t_symbol *symbol, uint64_t idx)
{
	t_arena	*input;
	t_arena	*tokens;
	t_token	*token;

	tokens = &c->arena[AT_TOKENS];
	input = &c->arena[AT_STRING];
	token = get_ptr_from_idx(tokens, symbol->token_idx);
	fprintf(out, "%lu %s(", idx, get_symbol_type_name(symbol->type));
	if (symbol->type == SYM_LINEBREAK && input->buf[token->offset] != '\n')
		fprintf(out, "epsilon");
	else
		print_escaped_str(out, input->buf + token->offset);
	fprintf(out, ") { node_idx = %lu flags = ", symbol->node_idx);
	if (token->flags)
	{
		fprintf(out, " ");
		print_flags(out, token->flags);
	}
	fprintf(out, " }\n");
}

void	print_tokens(t_ctx *c)
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
	fprintf(stderr, "\n--- tokens ---\n");
	i = 1;
	while (i <= count)
	{
		token = get_ptr_from_idx(tokens, i);
		print_token_line(stdout, c, token);
		++i;
	}
}

void	print_symbols(t_ctx *c, t_parser_state *parse)
{
	t_arena	*stack;
	t_symbol	*symbol;
	uint64_t	top;
	uint64_t	count;
	uint64_t	i;

	stack = &c->arena[AT_STACK];
	if (stack->cap == 0)
		return ;
	top = parse->stack_idx;
	if (top == 0)
		return ;
	count = (top - 1) / stack->stride + 1;
	if (count == 0)
		return ;
	fprintf(stderr, "\n--- symbols ---\n");
	i = 1;
	while (i <= count)
	{
		symbol = get_ptr_from_idx(stack, i);
		print_symbol_line(stdout, c, symbol, i);
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

void	print_nodes(t_ctx *c)
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
	fprintf(stderr, "\n--- nodes ---\n");
	i = 1;
	while (i <= count)
	{
		node = get_ptr_from_idx(commands, i);
		print_node_line(stderr, c, node, i);
		++i;
	}
}
