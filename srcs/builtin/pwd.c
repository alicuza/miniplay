/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+
	+:+     */
/*   By: nribakov <nribakov@student.42vienna.com    +#+  +:+
	+#+        */
/*                                                +#+#+#+#+#+
	+#+           */
/*   Created: 2026/06/06 16:21:09 by nribakov          #+#    #+#             */
/*   Updated: 2026/06/18 20:46:54 by nribakov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "env.h"
#include "minishell.h"

char	*get_pwd(t_ctx *c)
{
	char *pwd;
	pwd = env_get(&c->env, PWD);
	if (!pwd)
		pwd = getcwd(NULL, 0);
#ifdef DEBUG
	if (!pwd)
	{
		fprintf(stderr, "Error: failed to get cwd");
	}
#endif
	return (pwd);
}

int	pwd(t_ctx *c, t_command_ctx *command_ctx)
{
	char *pwd;

#ifdef DEBUG
	fprintf(stderr, "\nExecuting pwd:\n");
#endif


	(void)command_ctx;
	pwd = get_pwd(c);
	if (!pwd)
	{
		ft_putstr_fd("pwd: can't obtain pwd\n", STDERR_FILENO);
		return (EXIT_FAILURE);
	}
	printf("%s\n", pwd);
	free(pwd);
	return (EXIT_SUCCESS);
}
