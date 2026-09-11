#include "minishell.h"

uint64_t	reduce_redir_append(t_ctx *c, t_parser_state *parse,
		t_rule *rule)
{
	t_symbol	*list_sym;
	t_symbol	*redir_sym;

	list_sym = get_symbol_from_rhs(c, parse, rule, 0);
	redir_sym = get_symbol_from_rhs(c, parse, rule, 1);
	return (append_node_to_tail(c, list_sym->node_idx, redir_sym->node_idx));
}

uint64_t	reduce_io_file(t_ctx *c, t_parser_state *parse, t_rule *rule)
{
	t_symbol		*symbol;
	t_symbol		*op;
	t_node			*node;
	uint64_t		node_idx;

	symbol = get_symbol_from_rhs(c, parse, rule, 1);
	node_idx = symbol->node_idx;
	node = get_node_from_idx(c, node_idx);
	op = get_symbol_from_rhs(c, parse, rule, 0);
	if (op->type == SYM_LESS)
		node->flags |= REDIR_IN;
	else if (op->type == SYM_GREAT)
		node->flags |= REDIR_OUT;
	else
		node->flags |= REDIR_APPEND;
	return (node_idx);
}

static uint8_t	set_here_delim(t_ctx *c, t_token *token)
{
	char	*delim_word;
	uint8_t	flags;

	delim_word = get_token_body(c, token);
	delim_word = quote_remove_inplace(delim_word);
	flags = REDIR_HERE;
	if (token->flags & TKN_HAS_QUOTES)
		flags |= REDIR_HAS_QUOTES;
	return (flags);
}

uint64_t	reduce_io_here(t_ctx *c, t_parser_state *parse, t_rule *rule)
{
	t_token		*token;
	t_symbol	*symbol;
	t_node		*node;
	uint64_t	node_idx;
	uint8_t		flags;

	symbol = get_symbol_from_rhs(c, parse, rule, 1);
	token = get_token_from_idx(c, symbol->token_idx);
	flags = set_here_delim(c, token);
#ifdef DEBUG
	if (c->dbg.states & DBG_HEREDOC)
		print_here_saving();
#endif
	parse->flags |= PARSE_SAVE_TOKENS;
	node_idx = alloc_node(c, NODE_REDIR);
	node = get_node_from_idx(c, node_idx);
	node->flags = flags;
	node->data.redir.arena_offset = token->offset;
	return (node_idx);
}
