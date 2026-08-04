/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_processor.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nribakov <nribakov@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 13:10:11 by nribakov          #+#    #+#             */
/*   Updated: 2026/08/04 01:37:37 by nribakov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtin.h"
#include "minishell.h"

#define EQUAL 0

t_command_function	match_builtin(char *name)
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

/*
	Directly follows 2.9.1.4 Command Search and Execution
	TODO nik: maybe remember its location and need not search for the utility again unless the PATH variable has been the subject of an assignment. If the remembered location fails for a subsequent invocation,
		the shell shall repeat the search to find the new location for the utility,
		if any.
*/
int	command_search_and_execution(t_ctx *c, t_command_ctx *cmd_ctx)
{
	t_command_function	command;
	int					status;

	command = NULL;
	if (ft_strchr(cmd_ctx->pathname, '/') == NULL)
	{
		command = match_builtin(cmd_ctx->pathname);
		if (command != NULL)
			return (command(c, cmd_ctx));
		else
		{
			status = get_pathname(c, cmd_ctx);
			if (status == EXIT_FAILURE)
				return (exit_mem_issue());
			else if (status == EXIT_SUCCESS && cmd_ctx->pathname != NULL)
				return (execute_non_builtin(c, cmd_ctx));
			else
				return (127);
		}
	}
	else
		return (execute_non_builtin(c, cmd_ctx));
}

int	init_command(t_command_ctx *command, uint64_t argc)
{
	command->pathname = NULL;
	command->argc = argc;
	command->argv = malloc(sizeof(char *) * (argc + 1));
	if (command->argv == NULL)
		return (exit_mem_issue());
	command->argv[argc] = NULL;
	return (EXIT_SUCCESS);
}

// TODO "export " will create env with empty key
// TODO we have SYM_NEWLINE as last one, -1 to not cout it into argc
int	build_command(t_ctx *c, t_parser_state *parse)
{
	t_symbol		*sym;
	uint64_t		stack_idx;
	t_arena			*stack;
	t_command_ctx	command;
	t_token			*token;

	stack = &c->arena[AT_STACK];
	if (init_command(&command, parse->stack_idx - 1))
		return (EXIT_FAILURE);
	stack_idx = 1;
	sym = get_ptr_from_idx(stack, stack_idx);
	while (sym && sym->type == SYM_WORD)
	{
		token = get_ptr_from_idx(&c->arena[AT_TOKENS], sym->token_idx);
		if (command.pathname == NULL)
		{
			command.pathname = ft_strdup(c->arena[AT_STRING].buf
					+ token->offset);
			if (command.pathname == NULL)
				return (EXIT_FAILURE);
		}
		command.argv[stack_idx - 1] = c->arena[AT_STRING].buf + token->offset;
		stack_idx++;
		sym = get_ptr_from_idx(stack, stack_idx);
	}
	c->return_status = command_search_and_execution(c, &command);
	free(command.pathname);
	free(command.argv);
	return (EXIT_SUCCESS);
}
/*
TODO nik: Maybe part of reduction:
  - read 2.6.6 Pathname Expansion
	In the case of a simple command, the results usually include a list of pathnames and fields to be treated as a command name and arguments; see 2.9 Shell Commands.

The shell performs redirection (see 2.7 Redirection) and removes redirection operators and their operands from the parameter list.

The shell executes a function (see 2.9.5 Function Definition Command), built-in (see 2.15 Special Built-In Utilities), executable file, or script, giving the names of the arguments as positional parameters numbered 1 to n, and the name of the command (or in the case of a function within a script, the name of the script) as special parameter 0 (see 2.9.1.4 Command Search and Execution).

The shell optionally waits for the command to complete and collects the exit status (see 2.8.2 Exit Status for Commands).
*/
void	exec_list(t_ctx *c, uint64_t head_idx)
{
	t_node		*pipeline_node;

	pipeline_node = get_ptr_from_idx(&c->arena[AT_COMMAND], head_idx);
	while (pipeline_node)
	{
		// TODO nik: walk the arg chain and redirection chain of
		// pipeline_node->data.pipeline.command_head_idx and execute
		pipeline_node = pipeline_node->data.pipeline.next_idx
			? get_ptr_from_idx(&c->arena[AT_COMMAND],
				pipeline_node->data.pipeline.next_idx)
			: NULL;
	}
}
