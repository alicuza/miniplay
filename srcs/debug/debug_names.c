/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_names.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sancuta <sancuta@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/22 10:15:13 by sancuta           #+#    #+#             */
/*   Updated: 2026/08/08 12:44:25 by sancuta          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
	else if (type == SYM_ACCEPT)
		return ("SYM_ACCEPT");
	else if (type == SYM_PROGRAM)
		return ("SYM_PROGRAM");
	else if (type == SYM_COMPLETE_COMMANDS)
		return ("SYM_COMPLETE_COMMANDS");
	else if (type == SYM_AND_OR)
		return ("SYM_AND_OR");
	else if (type == SYM_PIPELINE)
		return ("SYM_PIPELINE");
	else if (type == SYM_COMMAND)
		return ("SYM_COMMAND");
	else if (type == SYM_SUBSHELL)
		return ("SYM_SUBSHELL");
	else if (type == SYM_COMPOUND_LIST)
		return ("SYM_COMPOUND_LIST");
	else if (type == SYM_TERM)
		return ("SYM_TERM");
	else if (type == SYM_SIMPLE_COMMAND)
		return ("SYM_SIMPLE_COMMAND");
	else if (type == SYM_CMD_NAME)
		return ("SYM_CMD_NAME");
	else if (type == SYM_CMD_WORD)
		return ("SYM_CMD_WORD");
	else if (type == SYM_CMD_PREFIX)
		return ("SYM_CMD_PREFIX");
	else if (type == SYM_CMD_SUFFIX)
		return ("SYM_CMD_SUFFIX");
	else if (type == SYM_REDIRECT_LIST)
		return ("SYM_REDIRECT_LIST");
	else if (type == SYM_IO_REDIRECT)
		return ("SYM_IO_REDIRECT");
	else if (type == SYM_IO_FILE)
		return ("SYM_IO_FILE");
	else if (type == SYM_FILENAME)
		return ("SYM_FILENAME");
	else if (type == SYM_IO_HERE)
		return ("SYM_IO_HERE");
	else if (type == SYM_HERE_END)
		return ("SYM_HERE_END");
	else if (type == SYM_NEWLINE_LIST)
		return ("SYM_NEWLINE_LIST");
	else if (type == SYM_LINEBREAK)
		return ("SYM_LINEBREAK");
	return ("UNKNOWN");
}

const char	*get_node_type_name(t_node_type type)
{
	if (type == NODE_DEFAULT)
		return ("NODE_DEFAULT");
	else if (type == NODE_PIPELINE)
		return ("NODE_PIPELINE");
	else if (type == NODE_COMMAND)
		return ("NODE_COMMAND");
	else if (type == NODE_ARG)
		return ("NODE_ARG");
	else if (type == NODE_REDIR)
		return ("NODE_REDIR");
	return ("UNKNOWN");
}

const char	*get_node_flag_name(uint32_t bit)
{
	if (bit == FLAG_AND_IF)
		return ("FLAG_AND_IF");
	else if (bit == FLAG_OR_IF)
		return ("FLAG_OR_IF");
	else if (bit == FLAG_SUBSHELL)
		return ("FLAG_SUBSHELL");
	else if (bit == REDIR_IN)
		return ("REDIR_IN");
	else if (bit == REDIR_OUT)
		return ("REDIR_OUT");
	else if (bit == REDIR_HERE)
		return ("REDIR_HERE");
	else if (bit == REDIR_APPEND)
		return ("REDIR_APPEND");
	else if (bit == REDIR_HAS_QUOTES)
		return ("REDIR_HAS_QUOTES");
	return ("UNKNOWN");
}

const char	*get_parse_flag_name(uint32_t bit)
{
	if (bit == PARSE_DONE)
		return ("PARSE_DONE");
	else if (bit == PARSE_SAVE_TOKENS)
		return ("PARSE_SAVE_TOKENS");
	else if (bit == PARSE_HAS_SAVED_TOKENS)
		return ("PARSE_HAS_SAVED_TOKENS");
	else if (bit == PARSE_HAS_LOOKAHEAD)
		return ("PARSE_HAS_LOOKAHEAD");
	else if (bit == PARSE_HERE_BODY)
		return ("PARSE_HERE_BODY");
	else if (bit == PARSE_ERROR)
		return ("PARSE_ERROR");
	return ("UNKNOWN");
}

const char	*get_lex_flag_name(uint32_t bit)
{
	if (bit == TKN_HAS_QUOTES)
		return ("TKN_HAS_QUOTES");
	else if (bit == TKN_HAS_EXPANSION)
		return ("TKN_HAS_EXPANSION");
	else if (bit == LEX_IS_BUILDING)
		return ("LEX_IS_BUILDING");
	else if (bit == LEX_AT_EOI)
		return ("LEX_AT_EOI");
	return ("UNKNOWN");
}
