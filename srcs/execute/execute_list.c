/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_list.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nribakov <nribakov@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 13:10:11 by nribakov          #+#    #+#             */
/*   Updated: 2026/09/07 22:48:55 by nribakov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_node	*get_next_with_or(t_ctx *c, t_node *current_pipeline_node) //TODO nik maybe infinite loop
{
	t_node	*pipeline_node;

	pipeline_node = get_ptr_from_idx(&c->arena[AT_COMMAND],
			current_pipeline_node->next_idx);
	while (pipeline_node->type == NODE_PIPELINE
		&& !(pipeline_node->flags & FLAG_OR_IF))
	{
		pipeline_node = get_ptr_from_idx(&c->arena[AT_COMMAND],
				pipeline_node->next_idx);
	}
	return (pipeline_node);
}

static t_node	*get_next_with_and(t_ctx *c, t_node *current_pipeline_node)
{
	t_node	*pipeline_node;

	pipeline_node = get_ptr_from_idx(&c->arena[AT_COMMAND],
			current_pipeline_node->next_idx);
	while (pipeline_node->type == NODE_PIPELINE
		&& !(pipeline_node->flags & FLAG_AND_IF))
	{
		pipeline_node = get_ptr_from_idx(&c->arena[AT_COMMAND],
				pipeline_node->next_idx);
	}
	return (pipeline_node);
}

static t_node	*get_next(t_ctx *c, t_node *current_pipeline_node)
{
	t_node	*pipeline_node;

	pipeline_node = get_ptr_from_idx(&c->arena[AT_COMMAND],
			current_pipeline_node->next_idx);
	if (pipeline_node->type != NODE_PIPELINE
		|| (!(pipeline_node->flags & FLAG_AND_IF)
			&& !(pipeline_node->flags & FLAG_OR_IF))
		|| ((pipeline_node->flags & FLAG_AND_IF)
			&& c->return_status == EXIT_SUCCESS)
		|| ((pipeline_node->flags & FLAG_OR_IF)
			&& c->return_status != EXIT_SUCCESS))
		return (pipeline_node);
	else if (pipeline_node->flags & FLAG_AND_IF)
		return (get_next_with_or(c, pipeline_node));
	else
		return (get_next_with_and(c, pipeline_node));
}

// TODO nik: set up https://www.gnu.org/software/bash/manual/bash.html#Signals-1
void	execute_list(t_ctx *c, uint64_t head_idx)
{
	t_node	*pipeline_node;
	t_node	*command_node;

	pipeline_node = get_ptr_from_idx(&c->arena[AT_COMMAND], head_idx);
	while (pipeline_node->type == NODE_PIPELINE)
	{
		command_node = get_ptr_from_idx(&c->arena[AT_COMMAND],
				pipeline_node->data.pipeline.command_head_idx);
		if (command_node->next_idx == 0)
		{
			c->return_status = execute_simple_command(c, command_node);
			if (c->return_status == EXIT_SUCCESS && c->pid_to_wait != -1)
				wait_return_status(c);
		}
		else
			execute_pipeline(c, pipeline_node);
		pipeline_node = get_next(c, pipeline_node);
	}
}
