/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_helpers.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sancuta <sancuta@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/09 17:29:40 by sancuta           #+#    #+#             */
/*   Updated: 2026/09/11 12:46:29 by sancuta          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static uint64_t	get_ifs_end_idx(const char *s, uint64_t i)
{
	while (s[i] && is_char_in_set(s[i], IFS))
		i++;
	return (i);
}

static uint64_t	delimit_segment(t_ctx *c, t_command_ctx *cmd,
		t_expand_state *exp, uint64_t i)
{
	t_arena	*fields;

	fields = &c->arena[AT_FIELDS];
	fields->buf[i] = '\0';
	if (i > exp->field.pos)
	{
		exp->field.len = i - exp->field.pos;
		delimit_field(c, cmd, exp);
	}
	else
		exp->flags &= ~EXP_HAS_FIELD;
	i = get_ifs_end_idx(fields->buf, i + 1);
	exp->field.pos = i;
	return (i);
}

void	append_segment(t_ctx *c, t_command_ctx *cmd,
		t_expand_state *exp, const char *val)
{
	uint64_t	vlen;
	uint64_t	span;
	uint64_t	i;

	vlen = ft_strlen(val);
	if (vlen == 0)
		return ;
	append_field(c, exp, val, vlen);
	span = exp->field.pos + exp->field.len - vlen;
	i = span;
	while (i < span + vlen)
	{
		if (is_char_in_set(val[i - span], IFS))
			i = delimit_segment(c, cmd, exp, i);
		else
		{
			exp->flags |= EXP_HAS_FIELD;
			i++;
		}
	}
}

void	scan_word(t_ctx *c, t_command_ctx *cmd,
		t_expand_state *exp, char *word)
{
	char	*src;

	src = word;
	while (*src)
	{
		if (exp->flags & EXP_IN_SQUOTE)
			src = handle_squote(c, exp, src);
		else if (exp->flags & EXP_IN_DQUOTE)
			src = handle_dquote(c, cmd, exp, src);
		else
			src = handle_unquoted(c, cmd, exp, src);
	}
	if (exp->flags & EXP_HAS_FIELD)
		delimit_field(c, cmd, exp);
}
