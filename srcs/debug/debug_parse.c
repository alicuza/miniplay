/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_parse.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sancuta <sancuta@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 11:08:25 by sancuta           #+#    #+#             */
/*   Updated: 2026-08-04 15:30:00 by nribakov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	print_symbol(t_ctx *c, t_symbol *symbol, uint64_t idx)
{
	t_arena	*input;
	t_arena	*tokens;
	t_token	*token;

	tokens = &c->arena[AT_TOKENS];
	input = &c->arena[AT_STRING];
	token = get_ptr_from_idx(tokens, symbol->token_idx);
	fprintf(stderr, "\n--- symbol ---\n");
	fprintf(stderr, "  [%lu]  %s(", idx, get_symbol_type_name(symbol->type));
	print_escaped_str(stderr, input->buf + token->offset);
	fprintf(stderr, ")  {  token_idx = %lu state = %u  node_idx = %lu  flags = ",
		symbol->token_idx, symbol->entry_state, symbol->node_idx);
	print_flags(stderr, token->flags);
	fprintf(stderr, "  }\n");
}

void	print_symbol_line(FILE *out, t_ctx *c, t_symbol *symbol, uint64_t idx)
{
	t_arena	*input;
	t_arena	*tokens;
	t_token	*token;

	tokens = &c->arena[AT_TOKENS];
	input = &c->arena[AT_STRING];
	token = get_ptr_from_idx(tokens, symbol->token_idx);
	fprintf(out, "[%lu] %s(", idx, get_symbol_type_name(symbol->type));
	print_escaped_str(out, input->buf + token->offset);
	fprintf(out, ") {  token_idx = %lu  state = %u  node_idx = %lu",
		symbol->token_idx, symbol->entry_state, symbol->node_idx);
	if (token->flags)
	{
		fprintf(out, "  flags = ");
		print_flags(out, token->flags);
	}
	fprintf(out, "  }\n");
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

void	print_node_line(FILE *out, t_ctx *c, t_node *node, uint64_t idx)
{
	t_arena	*strings;

	strings = &c->arena[AT_STRING];
	fprintf(out, "[%lu] %s { ", idx, get_node_type_name(node->type));
	if (node->type == NODE_PIPELINE)
	{
		fprintf(out, "command_head_idx = %lu  next_idx = %lu",
			node->data.pipeline.command_head_idx,
			node->data.pipeline.next_idx);
	}
	else if (node->type == NODE_COMMAND)
	{
		fprintf(out, "arg_head_idx = %lu  redir_head_idx = %lu  next = %lu",
			node->data.command.arg_head_idx,
			node->data.command.redir_head_idx,
			node->data.command.next);
	}
	else if (node->type == NODE_ARG)
	{
		fprintf(out, "arena_offset = %lu  next = %lu",
			node->data.arg.arena_offset, node->data.arg.next);
		if (node->data.arg.arena_offset)
		{
			fprintf(out, " ");
			print_escaped_str(out, strings->buf + node->data.arg.arena_offset);
		}
	}
	else if (node->type == NODE_REDIR)
	{
		fprintf(out, "arena_offset = %lu  next = %lu  fd = %d",
			node->data.redir.arena_offset,
			node->data.redir.next,
			node->data.redir.fd);
	}
	if (node->flags)
	{
		fprintf(out, "  flags = ");
		print_flags(out, node->flags);
	}
	fprintf(out, " }\n");
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
		print_node_line(stdout, c, node, i);
		++i;
	}
}
