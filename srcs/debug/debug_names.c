/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_names.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sancuta <sancuta@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/22 10:15:13 by sancuta           #+#    #+#             */
/*   Updated: 2026/08/01 16:14:34 by sancuta          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// TODO stefan: update naming, depending on what flags we keep

#include "minishell.h"

const char	*get_token_type_name(t_token_type type)
{
	if (type == TKN_NONE)
		return ("TKN_NONE");
	else if (type == TKN_EOF)
		return ("TKN_EOF");
	else if (type == TKN_WORD)
		return ("TKN_WORD");
	else if (type == TKN_OPERATOR)
		return ("TKN_OPERATOR");
	return ("UNKNOWN");
}

const char	*get_symbol_type_name(t_symbol_type type)
{
	if (type == SYM_EOF)
		return ("SYM_EOF");
	else if (type == SYM_WORD)
		return ("SYM_WORD");
	else if (type == SYM_NEWLINE)
		return ("SYM_NEWLINE");
	else if (type == SYM_PIPE)
		return ("SYM_PIPE");
	else if (type == SYM_LESS)
		return ("SYM_LESS");
	else if (type == SYM_GREAT)
		return ("SYM_GREAT");
	else if (type == SYM_DLESS)
		return ("SYM_DLESS");
	else if (type == SYM_DGREAT)
		return ("SYM_DGREAT");
	else if (type == SYM_AND_IF)
		return ("SYM_AND_IF");
	else if (type == SYM_OR_IF)
		return ("SYM_OR_IF");
	else if (type == SYM_OPAR)
		return ("SYM_OPAR");
	else if (type == SYM_CPAR)
		return ("SYM_CPAR");
	return ("UNKNOWN");
}

const char	*get_lex_flag_name(uint32_t bit)
{
	if (bit == TKN_HAS_QUOTES)
		return ("TKN_HAS_QUOTES");
	else if (bit == TKN_HAS_EXPANSION)
		return ("TKN_HAS_EXPANSION");
	else if (bit == TKN_IS_HERE_BODY)
		return ("TKN_IS_HERE_BODY");
	else if (bit == LEX_IS_BUILDING)
		return ("LEX_IS_BUILDING");
	else if (bit == LEX_AT_EOI)
		return ("LEX_AT_EOI");
	else
		return ("UNKNOWN");
}

const char	*get_parse_flag_name(uint32_t bit)
{
	if (bit == PARSE_DONE)
		return ("PARSE_DONE");
	else if (bit == PARSE_HAS_SAVED_TOKENS)
		return ("PARSE_HAS_SAVED_TOKENS");
	else if (bit == PARSE_SAVE_TOKENS)
		return ("PARSE_SAVE_TOKENS");
	else if (bit == PARSE_HERE_BODY)
		return ("PARSE_HERE_BODY");
	else if (bit == PARSE_ERROR)
		return ("PARSE_ERROR");
	else if (bit == PARSE_HAS_LOOKAHEAD)
		return ("PARSE_HAS_LOOKAHEAD");
	else if (bit == PARSE_LOOKAHEAD_IS_EOF)
		return ("PARSE_LOOKAHEAD_IS_EOF");
	else
		return ("UNKNOWN");
}
