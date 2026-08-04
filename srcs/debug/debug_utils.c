/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sancuta <sancuta@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 21:48:29 by sancuta           #+#    #+#             */
/*   Updated: 2026/07/22 10:15:47 by sancuta          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	print_char_info(unsigned char c)
{
	if (c == '\0')
		fprintf(stderr, "'\\0'(%u)", c);
	else if (c == '\n')
		fprintf(stderr, "'\\n'(%u)", c);
	else if (ft_isspace(c))
		fprintf(stderr, "' '(32)");
	else if (ft_isprint(c))
		fprintf(stderr, "'%c'(%u)", c, c);
	else
		fprintf(stderr, "'.'(%u)", c);
}

void	print_escaped_str(FILE* out, const char *s)
{
	unsigned char	c;

	while (*s)
	{
		c = (unsigned char)*s;
		if (c == '\n')
			fprintf(out, "\\n");
		else if (c == '\\')
			fputc('\\', out);
		else if (ft_isprint(c))
			fputc(c, out);
		else if (ft_isspace(c))
			fputc(' ', out);
		else
			fputc('.', out);
		++s;
	}
}

void	parse_debug_args(int argc, char **argv, t_ctx *c)
{
	size_t	len;
	int		i;

	i = 1;
	while (i < argc)
	{
		len = ft_strlen(argv[i]);
		if (!ft_strncmp(argv[i], "--no_exec", len))
			c->no_exec = true;
		else if (len > 8 && !ft_strncmp(argv[i], "--scope=", 8))
		{
			if (ft_strnstr(argv[i], "tokens", len))
				c->scope |= SCOPE_TOKENS;
			else if (ft_strnstr(argv[i], "stack", len))
				c->scope |= SCOPE_STACK;
			else if (ft_strnstr(argv[i], "command", len))
				c->scope |= SCOPE_COMMAND;
			if (!c->scope)
				fprintf(stderr, "--scope: '%s' matched no scope\n", argv[i] + 8);
		}
	++i;
	}
}
