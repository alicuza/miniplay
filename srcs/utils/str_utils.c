/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sancuta <sancuta@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 12:42:11 by sancuta           #+#    #+#             */
/*   Updated: 2026/09/11 12:42:15 by sancuta          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool	is_empty_str(char *str)
{
	return (str == NULL || str[0] == '\0');
}

bool	is_char_in_set(char c, const char *set)
{
	while (*set)
	{
		if (c == *set)
			return (true);
		++set;
	}
	return (false);
}

bool	is_str_in_set(char *c, const char **set)
{
	size_t	i;

	i = 0;
	while (set[i])
	{
		if (!ft_strncmp(c, set[i], 2))
			return (true);
		++i;
	}
	return (false);
}
