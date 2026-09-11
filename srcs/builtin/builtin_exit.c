#include "minishell.h"
#include <limits.h>

static int	ft_isnumber(char *str)
{
	while (*str && ft_isspace(*str))
	{
		str++;
	}
	if (*str == 0)
		return (0);
	if (*str && (*str == '-' || *str == '+'))
	{
		str++;
	}
	if (*str == 0)
		return (0);
	while (*str)
	{
		if (!ft_isdigit(*str))
			return (0);
		str++;
	}
	return (1);
}

static int exit_with_code(t_ctx *c, t_command_ctx *command_ctx)
{
	long int exit_code;

	if (ft_isnumber(command_ctx->argv[1]))
	{
		exit_code = ft_atol(command_ctx->argv[1]);
		if (exit_code < INT_MAX && exit_code > INT_MIN)
		{
			cleanup(c);
			free(command_ctx->pathname);
			exit(exit_code);
		}
	}
	ft_putstr_fd("exit: ", STDERR_FILENO);
	ft_putstr_fd((char *)command_ctx->argv[1], STDERR_FILENO);
	ft_putstr_fd(": numeric argument required", STDERR_FILENO);
	c->should_exit = true;
	return (2);
}

int	builtin_exit(t_ctx *c, t_command_ctx *command_ctx)
{
	if (command_ctx->argc > 2)
	{
		ft_putstr_fd("exit: too many arguments\n", STDERR_FILENO);
		return (2);
	}
	else if (command_ctx->argc == 2)
	{
		return (exit_with_code(c, command_ctx));
	}
	c->should_exit = true;
	return (EXIT_SUCCESS);
}
