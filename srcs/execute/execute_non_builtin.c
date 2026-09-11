#include "env.h"
#include "minishell.h"

static int	handle_dup2_error(t_ctx *c, t_command_ctx *cmd_ctx, char **envp)
{
	perror("dup2");
	child_cleanup_all(c, cmd_ctx, envp);
	exit(EXIT_FAILURE);
	return (EXIT_FAILURE);
}

// TODO: here we need to reset sigaction for SIGINT and SIGQUITE since we need to interapt child
static int	execute_in_child(t_ctx *c, t_command_ctx *cmd_ctx, char **envp,
		t_node *redir_node)
{
#ifdef DEBUG
	fprintf(stderr, "\nexecuting in child: %s\n", cmd_ctx->pathname);
#endif
	sig_set_default();
	if (process_redirection(c, redir_node) == EXIT_FAILURE)
	{
		child_cleanup_all(c, cmd_ctx, envp);
		exit(EXIT_FAILURE);
	}
	if (c->io_fd[0] != -1)
	{
		if (dup2(c->io_fd[0], 0) < 0)
			return (handle_dup2_error(c, cmd_ctx, envp));
	}
	if (c->io_fd[1] != -1)
	{
		if (dup2(c->io_fd[1], 1) < 0)
			return (handle_dup2_error(c, cmd_ctx, envp));
	}
	ft_close_fd(&c->io_fd[0]);
	ft_close_fd(&c->io_fd[1]);
	ft_close_fd(&c->pipe_fd[0]);
	ft_close_fd(&c->pipe_fd[1]);
	execve(cmd_ctx->pathname, cmd_ctx->argv, envp);
	return(exit_child(c, cmd_ctx, envp));
}

int	execute_non_builtin(t_ctx *c, t_command_ctx *cmd_ctx, t_node *redir_node)
{
	char **envp;
	pid_t pid;

	envp = env_to_envp(&c->env);
	if (envp == NULL)
		return (exit_mem_issue());
#ifdef DEBUG
	fprintf(stderr, "\nexecute_non_builtin: %s\n", cmd_ctx->pathname);
#endif
	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		c->should_exit = true;
		free_str_arr(envp);
		return (EXIT_FAILURE); 
	}
	if (pid == 0)
		return (execute_in_child(c, cmd_ctx, envp, redir_node));
	c->pid_to_wait = pid;
	close_io(c);
	free_str_arr(envp);
	return (EXIT_SUCCESS);
}
