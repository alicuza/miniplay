/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nribakov <nribakov@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 21:47:55 by sancuta           #+#    #+#             */
/*   Updated: 2026/07/24 15:07:31 by nribakov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
/*
PWD from env if he value is an absolute pathname of the current working directory that is no longer than {PATH_MAX} bytes including the terminating null byte, and the value does not contain any components that are dot or dot-dot
otherwice  pwd -P
, if there is insufficient permission on the current working directory, or on any parent of that directory, to determine what that pathname would be, the value of PWD is unspecified. Assignments to this variable may be ignored. If an application sets or unsets the value of PWD , the behaviors of the cd and pwd utilities are unspecified.
*/
static t_ctx	init_ctx(char **envp)
{
	t_ctx	c;

	ft_memset(&c, 0, sizeof(t_ctx));
	c.arena[AT_PROMPT] = arena_init(ARENA_SIZE, sizeof(char));
	c.arena[AT_STRING] = arena_init(ARENA_SIZE, sizeof(char));
	c.arena[AT_TOKENS] = arena_init(ARENA_SIZE, sizeof(t_token));
	c.arena[AT_STACK] = arena_init(ARENA_SIZE, sizeof(t_symbol));
	c.arena[AT_COMMAND] = arena_init(ARENA_SIZE, sizeof(t_node));
	if (init_env(&c.env, envp))
		printf("Error init_env");
	if (isatty(STDIN_FILENO))
		c.is_interactive = true;
	return (c);
}

int	cleanup(t_ctx	*c)
{ 
	arena_free(&c->arena[AT_STRING]);
	arena_free(&c->arena[AT_TOKENS]);
	arena_free(&c->arena[AT_STACK]);
	arena_free(&c->arena[AT_PROMPT]);
	arena_free(&c->arena[AT_COMMAND]);
	free_env(&c->env);
	return 0;
}

int	clear_arenas(t_ctx	*c)
{ 
	arena_clear(&c->arena[AT_STRING]);
	arena_clear(&c->arena[AT_TOKENS]);
	arena_clear(&c->arena[AT_STACK]);
	arena_clear(&c->arena[AT_PROMPT]);
	arena_clear(&c->arena[AT_COMMAND]);
	return 0;
}

int	main(int argc, char **argv, char **envp)
{
	t_ctx	c;
	t_parser_state	parse;

	(void)argc;
	(void)argv;
	c = init_ctx(envp);
#ifdef DEBUG
	parse_debug_args(argc, argv, &c);
#endif
	ft_memset(&parse, 0, sizeof(t_parser_state));
	while (true)
	{
		if (!get_user_input(&c, INPUT_DEFAULT))
			break ;
		if (!*(c.read_line))
		{
			free(c.read_line);
			continue ;
		}
#ifdef DEBUG
		if (c.dbg.states & DBG_PARSER)
		{
			fprintf(stderr, "\n--- read_line ---\n");
			fprintf(stderr, "%s\n", c.read_line);
		}
		if (c.dbg.arenas & DBG_ARENA_PROMPT)
		{
			fprintf(stderr, "\n--- prompt arena after get_prompt ---\n");
			print_arena(&c.arena[AT_PROMPT]);
		}
#endif
		parse_input(&c, &parse);
		if (parse.flags & PARSE_ERROR)
		{
			c.return_status = 2;
			reset_parser(&c, &parse);
		}
#ifdef DEBUG
		if (c.dbg.scope & SCOPE_TOKENS)
			print_tokens(stdout, &c);
		if (c.dbg.arenas & DBG_ARENA_TOKENS)
			print_arena(&c.arena[AT_TOKENS]);
		if (c.dbg.scope & SCOPE_STACK)
		{
			c.dbg.awaiting = true;
			print_stack(stdout, &c, &parse);
			c.dbg.awaiting = false;
		}
		if (c.dbg.arenas & DBG_ARENA_STACK)
			print_arena(&c.arena[AT_STACK]);
		if (c.dbg.scope & SCOPE_COMMAND)
			print_nodes(stdout, &c);
		if (c.dbg.arenas & DBG_ARENA_COMMAND)
			print_arena(&c.arena[AT_COMMAND]);
#endif
		free(c.read_line);
	}
	finalize_parse(&c, &parse);
	if (parse.flags & PARSE_ERROR)
		c.return_status = 2;
	cleanup(&c);
	return (c.return_status);
}
