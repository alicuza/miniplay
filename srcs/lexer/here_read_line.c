#include "minishell.h"

static void	warn_here_eof(char *here_end)
{
	ft_putstr_fd(SHELLNAME": warning: here-document ", STDERR_FILENO);
	ft_putstr_fd("delimited by end-of-file (wanted `", STDERR_FILENO);
	ft_putstr_fd(here_end, STDERR_FILENO);
	ft_putendl_fd("')", STDERR_FILENO);
}

bool	read_here_line(t_ctx *c, t_lexer_state *lex, char *here_end)
{
	get_user_input(c, INPUT_CONTINUATION);
	if (!c->read_line)
	{
		if (g_signal == SIGINT)
			lex->flags |= LEX_INTERRUPTED;
		else
			warn_here_eof(here_end);
		lex->flags |= LEX_AT_EOI;
		return (false);
	}
	ft_memset(lex, 0, sizeof(t_lexer_state));
	return (true);
}

static bool	is_delim_line(t_ctx *c, t_lexer_state *lex, char *here_end)
{
	char		*line;
	uint64_t	len;

	line = c->read_line + lex->char_idx;
	len = ft_strlen(here_end);
	return (!ft_strncmp(line, here_end, len) && line[len] == '\n');
}

bool	here_line_ends(t_ctx *c, t_lexer_state *lex, char *here_end)
{
	if (!is_delim_line(c, lex, here_end))
		return (false);
	consume_char(lex, ft_strlen(here_end) + 1);
	return (true);
}
