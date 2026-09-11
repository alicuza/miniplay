#include "minishell.h"

void	child_cleanup(t_ctx *c, t_command_ctx *cmd_ctx)
{
	cleanup(c);
	free(cmd_ctx->pathname);
}

static int	handle_dup2_error(t_ctx *c, t_command_ctx *cmd_ctx, int error_code)
{
	perror("dup2");
	child_cleanup(c, cmd_ctx);
	exit(EXIT_FAILURE);
	return (error_code);
}

// TODO: here we need to reset sigaction for SIGINT and SIGQUITE since we need to interapt child
static int	execute_in_child(t_ctx *c, t_command_ctx *cmd_ctx,
		t_command_function command, t_node *redir_node)
{
	int	result_code;

	result_code = 0;
	if (process_redirection(c, redir_node) == EXIT_FAILURE)
	{
		child_cleanup(c, cmd_ctx);
		exit(EXIT_FAILURE);
	}
	if (c->io_fd[0] != -1)
	{
		if (dup2(c->io_fd[0], 0) < 0)
			return (handle_dup2_error(c, cmd_ctx, EXIT_FAILURE));
	}
	if (c->io_fd[1] != -1)
	{
		if (dup2(c->io_fd[1], 1) < 0)
			return (handle_dup2_error(c, cmd_ctx, EXIT_FAILURE));
	}
	ft_close_fd(&c->io_fd[0]);
	ft_close_fd(&c->io_fd[1]);
	ft_close_fd(&c->pipe_fd[0]);
	ft_close_fd(&c->pipe_fd[1]);
	result_code = command(c, cmd_ctx);
	child_cleanup(c, cmd_ctx);
	exit(result_code);
	return (result_code);
}

int	execute_builtin_in_subshell(t_ctx *c, t_command_ctx *cmd_ctx,
		t_command_function command, t_node *redir_node)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return (EXIT_FAILURE);
	}
	if (pid == 0)
		return (execute_in_child(c, cmd_ctx, command, redir_node));
	c->pid_to_wait = pid;
	close_io(c);
	return (EXIT_SUCCESS);
}
