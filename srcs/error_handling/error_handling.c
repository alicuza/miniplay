#include "minishell.h"

int	exit_mem_issue(void)
{
	perror("Memory allocation error");
	return (EXIT_FAILURE);
}

int	handle_redirection_error(t_ctx *c, char *filename)
{
	(void)c;
	ft_putstr_fd("redirection: ", STDERR_FILENO);
	ft_putstr_fd((char *)filename, STDERR_FILENO);
	perror(": ");
	return (EXIT_FAILURE);
}

void	handle_pipe_error(t_ctx *c)
{
	perror("pipe");
	cleanup(c);
	exit(EXIT_FAILURE);
}

void	child_cleanup_all(t_ctx *c, t_command_ctx *cmd_ctx, char **envp)
{
	cleanup(c);
	free(cmd_ctx->pathname);
	free_str_arr(envp);
}

int	exit_child(t_ctx *c, t_command_ctx *cmd_ctx, char **envp)
{
	char	*error;
	int		error_code;

	error_code = errno;
	error = ft_strjoin("execve: ", cmd_ctx->pathname);
	if (error == NULL)
		perror("Memory allocation error");
	else
	{
		errno = error_code;
		perror(error);
	}
	child_cleanup_all(c, cmd_ctx, envp);
	exit(EXIT_FAILURE);
	return (EXIT_FAILURE);
}
