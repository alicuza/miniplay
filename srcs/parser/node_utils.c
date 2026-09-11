/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   node_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sancuta <sancuta@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/05 18:41:13 by sancuta           #+#    #+#             */
/*   Updated: 2026/09/05 18:41:17 by sancuta          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

uint64_t	alloc_node(t_ctx *c, t_node_type type)
{
	t_arena		*commands;
	t_node		*node;
	uint64_t	idx;

	commands = &c->arena[AT_COMMAND];
	idx = get_idx_from_offset(commands,
			arena_alloc(commands, sizeof(t_node), _Alignof(t_node)));
	node = get_ptr_from_idx(commands, idx);
	node->type = type;
	node->flags = 0;
	node->next_idx = 0;
	ft_memset(&node->data, 0, sizeof(node->data));
	return (idx);
}

t_node	*get_node_from_idx(t_ctx *c, uint64_t idx)
{
	t_arena	*commands;

	commands = &c->arena[AT_COMMAND];
	return (get_ptr_from_idx(commands, idx));
}

uint64_t	get_node_tail_idx(t_ctx *c, uint64_t head_idx)
{
	t_node		*node;
	uint64_t	tail_idx;

	node = get_node_from_idx(c, head_idx);
	tail_idx = head_idx;
	while (node->next_idx)
	{
		tail_idx = node->next_idx;
		node = get_node_from_idx(c, tail_idx);
	}
	return (tail_idx);
}

uint64_t	append_node_to_tail(t_ctx *c, uint64_t head_idx, uint64_t new_idx)
{
	t_node		*tail;
	uint64_t	tail_idx;

	if (!head_idx)
		return (new_idx);
	tail_idx = get_node_tail_idx(c, head_idx);
	tail = get_node_from_idx(c, tail_idx);
	tail->next_idx = new_idx;
	return (head_idx);
}
