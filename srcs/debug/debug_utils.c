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

static void	parse_flag_list(const char *spec, uint8_t *mask, const char **names,
		const uint8_t *bits, uint8_t all)
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
			while (names[i])
			{
				if (ft_strlen(names[i]) == len
					&& !ft_strncmp(spec + pos, names[i], len))
				{
					*mask |= bits[i];
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

void	parse_debug_args(int argc, char **argv, t_ctx *c)
{
	static const char	*scope_names[] = {"tokens", "stack", "command",
							"trace", NULL};
	static const uint8_t	scope_bits[] = {SCOPE_TOKENS, SCOPE_STACK,
							SCOPE_COMMAND, SCOPE_TRACE};
	static const char	*state_names[] = {"lexer", "parser", "here", NULL};
	static const uint8_t	state_bits[] = {DBG_LEXER, DBG_PARSER, DBG_HEREDOC};
	static const char	*arena_names[] = {"prompt", "string", "tokens", "stack",
							"command", NULL};
	static const uint8_t	arena_bits[] = {DBG_ARENA_PROMPT, DBG_ARENA_STRING,
							DBG_ARENA_TOKENS, DBG_ARENA_STACK, DBG_ARENA_COMMAND};
	size_t	len;
	int		i;
	bool	states_seen;
	bool	arenas_seen;

	states_seen = false;
	arenas_seen = false;
	i = 1;
	while (i < argc)
	{
		len = ft_strlen(argv[i]);
		if (!ft_strncmp(argv[i], "--no_exec", len))
			c->dbg.no_exec = true;
		else if (len > 8 && (!ft_strncmp(argv[i], "--scope=", 8)
				|| !ft_strncmp(argv[i], "--tests=", 8)))
		{
			parse_flag_list(argv[i] + 8, &c->dbg.scope, scope_names, scope_bits,
				SCOPE_ALL);
			if (!c->dbg.scope)
				fprintf(stderr, "%s: matched no scope\n", argv[i]);
		}
		else if (len > 9 && !ft_strncmp(argv[i], "--states=", 9))
		{
			states_seen = true;
			parse_flag_list(argv[i] + 9, &c->dbg.states, state_names, state_bits,
				DBG_ALL_STATES);
		}
		else if (len > 9 && !ft_strncmp(argv[i], "--arenas=", 9))
		{
			arenas_seen = true;
			parse_flag_list(argv[i] + 9, &c->dbg.arenas, arena_names, arena_bits,
				DBG_ARENA_ALL);
		}
		++i;
	}
	if (!states_seen)
		c->dbg.states = DBG_ALL_STATES;
	if (!arenas_seen)
		c->dbg.arenas = 0;
}
