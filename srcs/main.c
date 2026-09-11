/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nribakov <nribakov@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 21:47:55 by sancuta           #+#    #+#             */
/*   Updated: 2026/09/11 14:02:25 by sancuta          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_ctx	init_ctx(char **envp)
{
	t_ctx	c;

	ft_memset(&c, 0, sizeof(t_ctx));
	c.arena[AT_PROMPT] = arena_init(ARENA_SIZE, sizeof(char));
	c.arena[AT_STRING] = arena_init(ARENA_SIZE, sizeof(char));
	c.arena[AT_TOKENS] = arena_init(ARENA_SIZE, sizeof(t_token));
	c.arena[AT_STACK] = arena_init(ARENA_SIZE, sizeof(t_symbol));
	c.arena[AT_COMMAND] = arena_init(ARENA_SIZE, sizeof(t_node));
	c.arena[AT_FIELDS] = arena_init(ARENA_SIZE, sizeof(char));
	c.arena[AT_ARGV] = arena_init(ARENA_SIZE, sizeof(t_argv_slot));
	c.io_fd[0] = -1;
	c.io_fd[1] = -1;
	c.pipe_fd[0] = -1;
	c.pipe_fd[1] = -1;
	c.pid_to_wait = -1;
	init_input(&c);
	if (init_env(&c.env, envp) == EXIT_FAILURE)
	{
		msh_error_errno("init_env", "env");
		cleanup(&c);
		exit(EXIT_FAILURE);
	}
	return (c);
}

static void	shell_loop(t_ctx *c)
{
	t_parser_state	parse;

	while (true)
	{
		if (!get_user_input(c, INPUT_DEFAULT))
		{
			if (sig_consume_sigint(c))
				continue ;
			if (c->is_interactive)
				ft_putendl_fd("exit", STDERR_FILENO);
			break ;
		}
		if (!c->read_line || !*(c->read_line))
		{
			free(c->read_line);
			c->read_line = NULL;
			continue ;
		}
#ifdef DEBUG
		debug_print_read_line(c);
#endif
		parse = parse_input(c);
		if (parse.flags & PARSE_INTERRUPTED)
			sig_consume_sigint(c);
		else
		{
			if (parse.flags & PARSE_ERROR)
				c->return_status = 2;
			sig_reset_sigint();
		}
#ifdef DEBUG
		debug_print_after_parse(c, &parse);
#endif
		free(c->read_line);
		c->read_line = NULL;
		if (c->should_exit)
			break ;
	}
}

int	main(int argc, char **argv, char **envp)
{
	t_ctx	c;

	(void)argc;
	(void)argv;
	c = init_ctx(envp);
	if (sig_setup_handler(&c))
	{
		cleanup(&c);
		return (EXIT_FAILURE);
	}
#ifdef DEBUG
	parse_debug_args(argc, argv, &c);
#endif
	shell_loop(&c);
	cleanup(&c);
	return (c.return_status);
}
