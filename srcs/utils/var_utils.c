/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   var_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sancuta <sancuta@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 12:42:22 by sancuta           #+#    #+#             */
/*   Updated: 2026/09/11 12:42:24 by sancuta          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool	is_name_start(char c)
{
	return (ft_isalpha((unsigned char)c) || c == '_');
}

bool	is_name_body(char c)
{
	return (ft_isalnum((unsigned char)c) || c == '_');
}

bool	is_valid_var_name(char *name)
{
	if (!is_name_start(*name))
		return (false);
	name++;
	while (*name != '\0')
	{
		if (!is_name_body(*name))
			return (false);
		name++;
	}
	return (true);
}
