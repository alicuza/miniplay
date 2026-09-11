/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_helpers.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sancuta <sancuta@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/05 19:11:50 by sancuta           #+#    #+#             */
/*   Updated: 2026/09/05 19:11:53 by sancuta          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	sig_rl_event_hook(void)
{
	if (g_signal == SIGINT)
	{
		rl_replace_line("", 0);
		rl_done = 1;
	}
	return (0);
}

bool	sig_consume_sigint(t_ctx *c)
{
	if (g_signal != SIGINT)
		return (false);
	g_signal = 0;
	c->return_status = 128 + SIGINT;
	return (true);
}

void	sig_reset_sigint(void)
{
	if (g_signal == SIGINT)
		g_signal = 0;
}
