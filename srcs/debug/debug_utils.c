/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sancuta <sancuta@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 21:48:29 by sancuta           #+#    #+#             */
/*   Updated: 2026/08/08 13:29:03 by sancuta          ###   ########.fr       */
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

size_t	escape_into_buf(char *dst, size_t size, const char *src, size_t len)
{
	size_t	pos;
	size_t	i;
	unsigned char	c;

	pos = 0;
	i = 0;
	while (i < len && pos + 2 < size)
	{
		c = (unsigned char)src[i];
		if (c == '\n')
		{
			dst[pos++] = '\\';
			dst[pos++] = 'n';
		}
		else if (c == '\\')
		{
			dst[pos++] = '\\';
			dst[pos++] = '\\';
		}
		else if (ft_isprint(c))
			dst[pos++] = c;
		else if (ft_isspace(c))
			dst[pos++] = ' ';
		else
			dst[pos++] = '.';
		++i;
	}
	dst[pos] = '\0';
	return (pos);
}

void	print_escaped_str(FILE* out, const char *s)
{
	print_escaped_strn(out, s, ft_strlen(s));
}

void	print_escaped_strn(FILE *out, const char *s, size_t n)
{
	unsigned char	c;
	size_t			i;

	i = 0;
	while (i < n)
	{
		c = (unsigned char)s[i];
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
		++i;
	}
}

void	parse_flag_list(const char *spec, uint8_t *mask,
		const t_dbg_flag *flags, uint8_t all)
{
	uint64_t	len;
	uint64_t	i;
	uint64_t	pos;

	pos = 0;
	while (spec[pos])
	{
		len = 0;
		while (spec[pos + len] && spec[pos + len] != ',')
			++len;
		if (len == 3 && !ft_strncmp(spec + pos, "all", 3))
			*mask |= all;
		else if (len == 4 && !ft_strncmp(spec + pos, "none", 4))
			*mask = 0;
		else if (len == 2 && !ft_strncmp(spec + pos, "no", 2))
			*mask = 0;
		else
		{
			i = 0;
			while (flags[i].name)
			{
				if (ft_strlen(flags[i].name) == len
					&& !ft_strncmp(spec + pos, flags[i].name, len))
				{
					*mask |= flags[i].bit;
					break ;
				}
				++i;
			}
		}
		pos += len;
		if (spec[pos] == ',')
			++pos;
	}
}
