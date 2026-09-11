/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_parse.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sancuta <sancuta@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 11:08:25 by sancuta           #+#    #+#             */
/*   Updated: 2026/08/10 19:13:12 by sancuta          ###   ########.fr       */
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

static void	print_symbol_desc(FILE *out, t_ctx *c, t_symbol *symbol,
		uint64_t idx)
{
	t_arena	*tokens;
	t_arena	*input;
	t_token	*token;

	tokens = &c->arena[AT_TOKENS];
	input = &c->arena[AT_STRING];
	token = get_ptr_from_idx(tokens, symbol->token_idx);
	fprintf(out, "%lu %s(", idx, get_symbol_type_name(symbol->type));
	if (symbol->type == SYM_LINEBREAK && input->buf[token->offset] != '\n')
		fprintf(out, "epsilon");
	else if (symbol_has_content(symbol->type))
		print_escaped_str(out, input->buf + token->offset);
	else
		fprintf(out, "node %lu", symbol->node_idx);
	fprintf(out, ")");
}

void	print_symbol(FILE *out, t_ctx *c, t_symbol *symbol, uint64_t idx)
{
	t_arena	*tokens;
	t_token	*token;

	tokens = &c->arena[AT_TOKENS];
	token = get_ptr_from_idx(tokens, symbol->token_idx);
	print_symbol_desc(out, c, symbol, idx);
	fprintf(out, " { node_idx = %lu entry_state = %u flags = ",
		symbol->node_idx, symbol->entry_state);
	print_flags(out, token->flags);
	fprintf(out, " }\n");
}

static void	print_lookahead(FILE *out, t_ctx *c, t_parser_state *parse)
{
	t_token	*token;
	char	*body;

	if (parse->lookahead_type == SYM_EOF)
	{
		fprintf(out, "%s", DBG_LOOKAHEAD_EOF);
		return ;
	}
	token = get_ptr_from_idx(&c->arena[AT_TOKENS], parse->token_idx);
	body = get_ptr_from_offset(&c->arena[AT_STRING], token->offset);
	fprintf(out, "%s(", get_symbol_type_name(classify_token(c, token)));
	print_escaped_str(out, body);
	fprintf(out, ")");
}

void	print_stack(FILE *out, t_ctx *c, t_parser_state *parse)
{
	uint64_t	phys;
	t_symbol	*symbol;
	t_arena		*stack;

	stack = &c->arena[AT_STACK];
	fprintf(out, "\n--- stack ---\n--- top -----\n");
	phys = parse->stack_idx;
	while (phys)
	{
		symbol = get_ptr_from_idx(stack, phys);
		print_symbol(out, c, symbol, phys);
		--phys;
	}
	fprintf(out, "--- bottom ----\n");
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

void	print_node_flags(FILE *out, uint8_t flags)
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
			node->next_idx,
			node->data.pipeline.command_head_idx);
	}
	else if (node->type == NODE_COMMAND)
	{
		fprintf(out, " [next %lu] [arg_head %lu] [redir_head %lu]",
			node->next_idx,
			node->data.command.arg_head_idx,
			node->data.command.redir_head_idx);
	}
	else if (node->type == NODE_ARG)
	{
		fprintf(out, "(");
		if (node->data.arg.arena_offset)
			print_escaped_str(out, strings->buf + node->data.arg.arena_offset);
		fprintf(out, ") [next %lu]", node->next_idx);
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
		fprintf(out, ") [next %lu]", node->next_idx);
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

/* -------- parser trace: one compact step line per shift/reduce ----------- */
/* [la] <lookahead> [shift N] | [reduce N] [goto N] | [accept] */
static void	fprint_step(FILE *out, t_ctx *c, t_parser_state *parse,
		t_debug_step step)
{
	fprintf(out, "[state %d] [la] ", step.from);
	print_lookahead(out, c, parse);
	if (step.kind == LALR_SHIFT)
		fprintf(out, " [shift %d]\n", parse->state);
	else if (step.kind == LALR_REDUCE)
		fprintf(out, " [reduce %d] [goto %d]\n", step.rule, parse->state);
	else
		fprintf(out, " [accept]\n");
}

void	print_step(t_ctx *c, t_parser_state *parse, t_debug_step step)
{
	if ((c->dbg.states & DBG_PARSER) && (c->dbg.parser & DBG_SHOW_ACTION))
		fprint_step(stderr, c, parse, step);
	if (c->dbg.scope & SCOPE_TRACE)
		fprint_step(stdout, c, parse, step);
}

void	log_rhs_symbols(t_ctx *c, t_parser_state *parse, t_rule *rule)
{
	uint32_t	rhs;
	t_symbol	*symbol;

	rhs = 0;
	while (rhs < rule->rhs_len)
	{
		symbol = get_symbol_from_rhs(c, parse, rule, rhs);
		if (symbol->node_idx)
		{
			fprintf(stderr, "[rhs] ");
			print_node_line(stderr, c,
				get_node_from_idx(c, symbol->node_idx),
				symbol->node_idx);
		}
		++rhs;
	}
}

void	debug_parse_header(t_parser_state *parse)
{
	uint32_t	bit;
	uint8_t		flags;

	fprintf(stderr, "\n--- parse ---\n");
	if (parse->flags & PARSE_HAS_LOOKAHEAD)
		fprintf(stderr, "[lookahead] ");
	fprintf(stderr, "token_idx = %lu  flags =", parse->token_idx);
	flags = parse->flags;
	bit = 1;
	while (bit && !(flags & bit))
		bit <<= 1;
	if (!bit)
		fprintf(stderr, " (none)");
	while (bit)
	{
		if (flags & bit)
		{
			fprintf(stderr, " %s", get_parse_flag_name(bit));
			flags ^= bit;
		}
		bit <<= 1;
	}
	fprintf(stderr, "\n");
}

void	debug_parse_arenas(t_ctx *c)
{
	if (c->dbg.arenas & DBG_ARENA_STRING)
		print_arena(&c->arena[AT_STRING]);
	if (c->dbg.arenas & DBG_ARENA_TOKENS)
		print_arena(&c->arena[AT_TOKENS]);
}

void	debug_parse_action(t_ctx *c, t_parser_state *parse,
		t_lalr_action action)
{
	if (!(c->dbg.states & DBG_PARSER))
		return ;
	if (c->dbg.parser & DBG_SHOW_STACK)
		print_stack(stderr, c, parse);
	if ((c->dbg.parser & DBG_SHOW_NODES)
		&& (action == LALR_REDUCE || action == LALR_ACCEPT))
		print_nodes(stderr, c);
}
