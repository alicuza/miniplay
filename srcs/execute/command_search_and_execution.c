#include "builtin.h"
#include "minishell.h"

#define EQUAL 0

static t_command_function	match_builtin(char *name)
{
	if (ft_strncmp(name, ENV, 4) == EQUAL)
		return (&env);
	else if (ft_strncmp(name, PWD, 4) == EQUAL)
		return (&pwd);
	else if (ft_strncmp(name, EXIT, 5) == EQUAL)
		return (&builtin_exit);
	else if (ft_strncmp(name, CD, 3) == EQUAL)
		return (&cd);
	else if (ft_strncmp(name, EXPORT, 7) == EQUAL)
		return (&builtin_export);
	else if (ft_strncmp(name, UNSET, 6) == EQUAL)
		return (&unset);
	else if (ft_strncmp(name, B_ECHO, 7) == EQUAL)
		return (&echo);
	else
		return (NULL);
}

int	command_search_and_execution(t_ctx *c, t_command_ctx *cmd_ctx,
		t_node *redir_node)
{
	t_command_function	command;
	int					status;

	command = NULL;
	if (ft_strchr(cmd_ctx->pathname, '/') == NULL)
	{
		command = match_builtin(cmd_ctx->pathname);
		if (command != NULL)
		{
			if (!c->is_pipe
				&& process_redirection(c, redir_node) == EXIT_FAILURE)
				return (1);
			return (execute_builtin(c, cmd_ctx, command, redir_node));
		}
		else
		{
			status = get_pathname(c, cmd_ctx);
			if (status == EXIT_FAILURE)
				return (exit_mem_issue());
			else if (status == EXIT_SUCCESS && cmd_ctx->pathname != NULL)
				return (execute_non_builtin(c, cmd_ctx, redir_node));
			else
				return (127);
		}
	}
	else
		return (execute_non_builtin(c, cmd_ctx, redir_node));
}
