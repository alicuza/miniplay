/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sancuta <sancuta@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 12:47:24 by sancuta           #+#    #+#             */
/*   Updated: 2026/09/11 12:47:25 by sancuta          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool	is_expansion_start(char *buf, uint64_t idx)
{
	return (buf[idx] == '$' && (is_name_start(buf[idx + 1])
			|| is_char_in_set(buf[idx + 1], SPECIAL_PARAM_SET)));
}

uint64_t	get_expansion_len(char *expansion)
{
	uint64_t	i;

	if (is_char_in_set(expansion[1], SPECIAL_PARAM_SET))
		return (2);
	i = 2;
	while (expansion[i] && is_name_body(expansion[i]))
		++i;
	return (i);
}
