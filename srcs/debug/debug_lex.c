/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_lex.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sancuta <sancuta@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 11:08:25 by sancuta           #+#    #+#             */
/*   Updated: 2026/07/22 09:53:17 by sancuta          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	print_lex_rule(t_ctx *c, int n)
{
	if (c->dbg.states & DBG_LEXER)
		fprintf(stderr, "rule %d\n", n);
}

void	print_flags(FILE *out, uint32_t flags)
{
	uint32_t	bit;

	if (!flags)
	{
		fprintf(out, "(no flags set)");
		return ;
	}
	bit = 1;
	while (!(flags & bit))
		bit <<= 1;
	fprintf(out, "%s", get_lex_flag_name(bit));
	flags ^= bit;
	while (flags)
	{
		bit <<= 1;
		if (flags & bit)
		{
			fprintf(out, " | %s", get_lex_flag_name(bit));
			flags ^= bit;
		}
	}
}

void	print_lex_state(t_ctx *c, t_lexer_state *l)
{
	unsigned char	ch;

	ch = (unsigned char)c->read_line[l->char_idx];
	fprintf(stderr, "\n--- lex ---\n");
	fprintf(stderr, "  [%lu] ", l->char_idx);
	print_char_info(ch);
	fprintf(stderr, "  %s", get_token_type_name(l->type));
	if (l->flags & LEX_IS_BUILDING)
	{
		fprintf(stderr, "(");
		print_escaped_strn(stderr, c->read_line + l->token.pos, l->token.len);
		fprintf(stderr, ")");
	}
	fprintf(stderr, "  {  token.pos = %lu  token.len = %lu  flags = ",
		l->token.pos, l->token.len);
	print_flags(stderr, l->flags);
	fprintf(stderr, "  }");
	fprintf(stderr, "\n");
}

void	print_token(FILE *out, t_ctx *c, t_token *token)
{
	t_arena	*input;

	input = &(c->arena[AT_STRING]);
	fprintf(stderr, "\n--- token ---\n");
	fprintf(out, "  %s(", get_token_type_name(token->type));
	print_escaped_str(out, input->buf + token->offset);
	fprintf(out, ")");
	fprintf(stderr, "  {  offset = %lu  flags =", token->offset);
	fprintf(out, " ");
	print_flags(out, token->flags);
	fprintf(stderr, "  }");
	fprintf(out, "\n");
}

void	print_token_line(FILE *out, t_ctx *c, t_token *token)
{
	t_arena	*input;
	uint32_t	bit;

	input = &(c->arena[AT_STRING]);
	fprintf(out, "%s(", get_token_type_name(token->type));
	print_escaped_str(out, input->buf + token->offset);
	fprintf(out, ")");
	if (token->flags)
	{
		bit = 1;
		while (bit && !(token->flags & bit))
			bit <<= 1;
		while (bit)
		{
			if (token->flags & bit)
			{
				fprintf(out, " %s", get_lex_flag_name(bit));
				token->flags ^= bit;
			}
			bit <<= 1;
		}
	}
	fprintf(out, "\n");
}
