#include "minishell.h"

static void	write_here(t_ctx *c, int fd, char *buf, uint64_t len)
{
	if (write(fd, buf, len) == -1)
		fatal(c, "heredoc", strerror(errno));
}

static char	*get_expansion_value(t_ctx *c, char *name)
{
	if (*name == '?')
		return (ft_itoa(c->return_status));
	return (env_get(&c->env, name));
}

static uint64_t	expand_here_var(t_ctx *c, int fd, char *line, uint64_t i)
{
	uint64_t	end;
	char		*name;
	char		*val;

	end = get_expansion_len(line + i);
	name = ft_substr(line + i + 1, 0, end - 1);
	if (!name)
	{
		fatal(c, "heredoc", "out of memory");
		return (i + end);
	}
	val = get_expansion_value(c, name);
	free(name);
	if (val)
	{
		write_here(c, fd, val, ft_strlen(val));
		free(val);
	}
	return (i + end);
}

static void	expand_here_line(t_ctx *c, int fd, char *line, uint64_t len)
{
	uint64_t	i;
	uint64_t	start;

	i = 0;
	start = 0;
	while (i < len)
	{
		if (is_expansion_start(line, i))
		{
			write_here(c, fd, line + start, i - start);
			i = expand_here_var(c, fd, line, i);
			start = i;
		}
		else
			++i;
	}
	write_here(c, fd, line + start, len - start + 1);
}

void	write_here_line(t_ctx *c, int fd, t_lexer_state *lex, t_node *node)
{
	char		*line;
	uint64_t	len;

	line = c->read_line + lex->char_idx;
	len = word_len(line, '\n');
	if (node->flags & REDIR_HAS_QUOTES)
		write_here(c, fd, line, len + 1);
	else
		expand_here_line(c, fd, line, len);
	consume_char(lex, len + 1);
}
