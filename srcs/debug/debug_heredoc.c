#include "minishell.h"

void	print_here_reading(t_ctx *c, t_parser_state *parse)
{
	t_arena	*strings;
	t_node	*node;
	char	*here_end;

	(void)parse;
	strings = &c->arena[AT_STRING];
	node = get_node_from_idx(c, get_symbol_from_top(c, 1)->node_idx);
	here_end = get_ptr_from_offset(strings, node->data.redir.arena_offset);
	fprintf(stderr, "\n--- heredoc ---\n");
	fprintf(stderr, "  [reading] body until \"");
	print_escaped_str(stderr, here_end);
	fprintf(stderr, "\"\n");
}

void	print_here_line(t_ctx *c, t_lexer_state *lex, uint64_t line, uint64_t len)
{
	fprintf(stderr, "  [line %lu] \"", line);
	print_escaped_strn(stderr, c->read_line + lex->char_idx, len);
	fprintf(stderr, "\"\n");
}

void	print_here_stored(t_parser_state *parse, uint64_t token_idx)
{
	(void)parse;
	fprintf(stderr, "  [stored] token_idx %lu\n", token_idx);
}

void	print_here_saving(void)
{
	fprintf(stderr, "\n--- heredoc ---\n");
	fprintf(stderr, "  [saving] entering SAVE_TOKENS mode\n");
}

void	print_here_replay(uint64_t idx)
{
	fprintf(stderr, "\n--- heredoc ---\n");
	fprintf(stderr, "  [replay] token %lu\n", idx);
}
