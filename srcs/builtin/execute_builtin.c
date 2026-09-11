#include "minishell.h"

int* save_fds()
{
	int	*saved_fds;
	
	saved_fds = malloc(sizeof(int) * 2);
	if(saved_fds == NULL)
		return NULL;
	saved_fds[0] = -1;
	saved_fds[1] = -1;
	saved_fds[0] = dup(0);
	if(saved_fds[0] < 0)
	{
		free(saved_fds);
		return(NULL);
	}
	saved_fds[1] = dup(1);
	if(saved_fds[1] < 0)
	{
		close(saved_fds[0]);
		free(saved_fds);
		return(NULL);
	}
	return(saved_fds);
}

static int	handle_dup2_error(t_ctx *c, t_command_ctx *cmd_ctx, int	*saved_fds)
{
	perror("dup2");
	cleanup(c);
	free(cmd_ctx->pathname);
	close(saved_fds[0]);
	close(saved_fds[1]);
	free(saved_fds);
	exit(EXIT_FAILURE);
	return (EXIT_FAILURE);
}
int reset_to_saved_fd(t_ctx *c, t_command_ctx *cmd_ctx, int	*saved_fds)
{
	struct stat	buf;

	if (fstat(STDIN_FILENO , &buf) != -1)
		if(dup2(saved_fds[0], 0) < 0)
			return (handle_dup2_error(c, cmd_ctx, saved_fds));
	if (fstat(STDOUT_FILENO , &buf) != -1)
		if(dup2(saved_fds[1], 1) < 0)
			return (handle_dup2_error(c, cmd_ctx, saved_fds));
	close(saved_fds[0]);
	close(saved_fds[1]);
	free(saved_fds);
	return (EXIT_SUCCESS);
}

static int	execute(t_ctx *c, t_command_ctx *cmd_ctx,
		t_command_function command)
{
	int	result_code;
	int	*saved_fds;

	result_code = 0;
	saved_fds = save_fds();
	if(saved_fds == NULL)
		return(EXIT_FAILURE);
	if (c->io_fd[0] != -1)
	{
		if(dup2(c->io_fd[0], 0) < 0)
			return (handle_dup2_error(c, cmd_ctx, saved_fds));		
		ft_close_fd(&c->io_fd[0]);
	}
	if (c->io_fd[1] != -1)
	{
		if(dup2(c->io_fd[1], 1) < 0)
			return (handle_dup2_error(c, cmd_ctx, saved_fds));
		ft_close_fd(&c->io_fd[1]);
	}
	result_code = command(c, cmd_ctx);
	reset_to_saved_fd(c, cmd_ctx, saved_fds);
	return (result_code);
}

int	execute_builtin(t_ctx *c, t_command_ctx *cmd_ctx,
		t_command_function command, t_node *redir_node)
{
	if (c->is_pipe)
		execute_builtin_in_subshell(c, cmd_ctx, command, redir_node);
	else
		execute(c, cmd_ctx, command);
	return (0);
}
