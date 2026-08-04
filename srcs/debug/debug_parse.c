/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_parse.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sancuta <sancuta@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 11:08:25 by sancuta           #+#    #+#             */
/*   Updated: 2026-08-04 14:25:00 by nribakov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	print_symbol(t_ctx *c, t_symbol *symbol, uint64_t idx)
{
	t_arena	*input;
	t_arena	*tokens;
	t_token	*token;

	tokens = &c->arena[AT_TOKENS];
	input = &c->arena[AT_STRING];
	token = get_ptr_from_idx(tokens, symbol->token_idx);
	fprintf(stderr, "\n--- symbol ---\n");
	fprintf(stderr, "  [%lu]  %s(", idx, get_symbol_type_name(symbol->type));
	print_escaped_str(stderr, input->buf + token->offset);
	fprintf(stderr, ")  {  token_idx = %lu state = %u  node_idx = %lu  flags = ",
		symbol->token_idx, symbol->entry_state, symbol->node_idx);
	print_flags(stderr, token->flags);
	fprintf(stderr, "  }\n");
}

void	print_tokens(t_ctx *c)
{
	t_arena	*tokens;
	t_token	*token;
	uint64_t	count;
	uint64_t	i;

	tokens = &c->arena[AT_TOKENS];
	if (tokens->cap == 0)
		return ;
	count = (tokens->offset - tokens->stride) / tokens->stride;
	if (count == 0)
		return ;
	fprintf(stderr, "\n--- tokens ---\n");
	i = 1;
	while (i <= count)
	{
		token = get_ptr_from_idx(tokens, i);
		print_token_line(stdout, c, token);
		++i;
	}
}
