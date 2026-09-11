/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_setup.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sancuta <sancuta@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/05 19:11:15 by sancuta           #+#    #+#             */
/*   Updated: 2026/09/06 13:27:39 by sancuta          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t	g_signal = 0;

int	sig_setup_handler(t_ctx *c)
{
	if (!c->is_interactive)
		return (sig_set_default());
	rl_event_hook = &sig_rl_event_hook;
	return (sig_set_interactive());
}

static void	sig_record_sigint(int signum)
{
	g_signal = signum;
}

static int	set_action(int signum, void (*handler)(int))
{
	struct sigaction	act;

	ft_memset(&act, 0, sizeof(act));
	act.sa_handler = handler;
	act.sa_flags = 0;
	if (sigemptyset(&act.sa_mask) == -1)
		return (EXIT_FAILURE);
	if (sigaction(signum, &act, NULL) == -1)
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

int	sig_set_interactive(void)
{
	if (set_action(SIGINT, &sig_record_sigint)
		|| set_action(SIGQUIT, SIG_IGN))
		return (msh_error_errno("sigaction", "interactive"));
	return (EXIT_SUCCESS);
}

int	sig_set_default(void)
{
	if (set_action(SIGINT, SIG_DFL) || set_action(SIGQUIT, SIG_DFL))
		return (msh_error_errno("sigaction", "default"));
	return (EXIT_SUCCESS);
}
