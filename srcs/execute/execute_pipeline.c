#include "minishell.h"

void	execute_pipeline(t_ctx *c, t_node *pipeline_node)
{
	t_node		*command_node;
	int			result;

	c->is_pipe = true;
	result = EXIT_SUCCESS;
	command_node = get_ptr_from_idx(&c->arena[AT_COMMAND],
			pipeline_node->data.pipeline.command_head_idx);
	while (command_node->type == NODE_COMMAND)
	{
		if (command_node->next_idx != 0)
		{
			if (pipe(c->pipe_fd) == -1)
			{
				ft_close_fd(&c->io_fd[0]);
				ft_close_fd(&c->io_fd[1]);
				return (handle_pipe_error(c));
			}
			c->io_fd[1] = c->pipe_fd[1];
			c->pipe_fd[1] = -1;
			result = execute_simple_command(c, command_node);
			if (c->should_exit)
				return ;
			ft_close_fd(&c->io_fd[0]);
			ft_close_fd(&c->io_fd[1]);
			c->io_fd[0] = c->pipe_fd[0];
			c->pipe_fd[0] = -1;
		}
		else
		{
			result = execute_simple_command(c, command_node);
			if (c->should_exit)
				return ;
		}
		command_node = get_ptr_from_idx(&c->arena[AT_COMMAND],
				command_node->next_idx);
	}
	c->return_status = result;
	if (c->return_status == EXIT_SUCCESS && c->pid_to_wait != -1)
		wait_return_status(c);
	c->is_pipe = false;
}
