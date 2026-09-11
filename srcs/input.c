/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nribakov <nribakov@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/25 12:45:50 by sancuta           #+#    #+#             */
/*   Updated: 2026/09/11 14:09:29 by sancuta          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*read_interactive(t_ctx *c, bool is_continuation)
{
	char	*prompt;
	char	*line;

	rl_outstream = stderr;
	if (is_continuation)
		prompt = GREEN"> "RESET;
	else
		prompt = get_prompt(c, true);
	line = readline(prompt);
	if (!is_empty_str(line))
		add_history(line);
	if (g_signal == SIGINT)
		return (free(line), NULL);
	line = ft_strjoin(line, "\n");
	return (line);
}

static char	*read_noninteractive(t_ctx *c, bool is_continuation)
{
	char	*line;

	(void)c;
	(void)is_continuation;
	line = get_next_line(STDIN_FILENO);
	return (line);
}

void	init_input(t_ctx *c)
{
	if (isatty(STDIN_FILENO))
	{
		c->is_interactive = true;
		c->read_handler = read_interactive;
	}
	else
		c->read_handler = read_noninteractive;
}

char	*get_user_input(t_ctx *c, bool is_continuation)
{
	char		*read_line;
	struct stat	buf;

	free(c->read_line);
	c->read_line = NULL;
	if (fstat(STDIN_FILENO , &buf) == -1)
		return (0);
	read_line = c->read_handler(c, is_continuation);
	return (free(read_line), c->read_line);
}
