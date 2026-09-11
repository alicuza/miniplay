#include "minishell.h"

int	msh_error(char *where, char *what, char *why)
{
	ft_putstr_fd(SHELLNAME": ", STDERR_FILENO);
	if (where)
	{
		ft_putstr_fd(where, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
	}
	if (what)
	{
		ft_putstr_fd(what, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
	}
	ft_putendl_fd(why, STDERR_FILENO);
	return (1);
}

int	msh_error_errno(char *where, char *what)
{
	return (msh_error(where, what, strerror(errno)));
}

void	fatal(t_ctx *c, char *where, char *why)
{
	if (why)
		msh_error(where, NULL, why);
	free(c->read_line);
	c->read_line = NULL;
	cleanup(c);
	exit(EXIT_FAILURE);
}
