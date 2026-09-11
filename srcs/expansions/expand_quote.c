/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_quote.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sancuta <sancuta@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/09 17:29:55 by sancuta           #+#    #+#             */
/*   Updated: 2026/09/11 12:46:40 by sancuta          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*open_quote(t_ctx *c, t_expand_state *exp, char *src)
{
	if (!(exp->flags & EXP_HAS_FIELD))
		append_field(c, exp, "", 0);
	if (*src == '\'')
		exp->flags |= EXP_IN_SQUOTE;
	else
		exp->flags |= EXP_IN_DQUOTE;
	return (src + 1);
}

static bool	has_closing_quote(char *word, uint64_t i)
{
	char	quote;

	quote = word[i];
	if (quote != '\'' && quote != '"')
		return (false);
	i++;
	while (word[i] && word[i] != quote)
		i++;
	return (word[i] == quote);
}

char	*handle_squote(t_ctx *c, t_expand_state *exp, char *src)
{
	uint64_t	i;

	if (*src == '\'')
	{
		exp->flags &= ~EXP_IN_SQUOTE;
		return (src + 1);
	}
	i = 0;
	while (src[i] && src[i] != '\'')
		i++;
	append_field(c, exp, src, i);
	return (src + i);
}

char	*handle_dquote(t_ctx *c, t_command_ctx *cmd,
		t_expand_state *exp, char *src)
{
	uint64_t	i;

	if (*src == '"')
	{
		exp->flags &= ~EXP_IN_DQUOTE;
		return (src + 1);
	}
	if (is_expansion_start(src, 0))
		return (expand_var(c, cmd, exp, src));
	i = 0;
	while (src[i] && src[i] != '"' && !is_expansion_start(src, i))
		i++;
	append_field(c, exp, src, i);
	return (src + i);
}

char	*handle_unquoted(t_ctx *c, t_command_ctx *cmd,
		t_expand_state *exp, char *src)
{
	uint64_t	i;

	if (has_closing_quote(src, 0))
		return (open_quote(c, exp, src));
	if (is_expansion_start(src, 0))
		return (expand_var(c, cmd, exp, src));
	i = 0;
	while (src[i] && !has_closing_quote(src, i)
		&& !is_expansion_start(src, i))
		i++;
	append_field(c, exp, src, i);
	return (src + i);
}
