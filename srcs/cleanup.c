#include "minishell.h"

int	cleanup(t_ctx *c)
{
	struct stat		buf;
	t_arena_type	i;

	i = 0;
	while (i < AT_COUNT)
		arena_free(&c->arena[i]);
	free(c->read_line);
	free_env(&c->env);
	ft_close_fd(&c->io_fd[0]);
	ft_close_fd(&c->io_fd[1]);
	ft_close_fd(&c->pipe_fd[0]);
	ft_close_fd(&c->pipe_fd[1]);
	if (fstat(STDIN_FILENO , &buf) != -1)
		close(0);
	if (fstat(STDOUT_FILENO , &buf) != -1)
		close(1);
	if (fstat(STDERR_FILENO , &buf) != -1)
		close(2);
	return (0);
}

void	close_io(t_ctx *c)
{
	ft_close_fd(&c->io_fd[0]);
	ft_close_fd(&c->io_fd[1]);
}

void	free_str_arr(char **val)
{
	int	i;

	i = 0;
	while (val[i] != NULL)
	{
		free(val[i]);
		i++;
	}
	free(val);
}
