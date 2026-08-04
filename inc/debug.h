/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nribakov <nribakov@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 21:48:29 by sancuta           #+#    #+#             */
/*   Updated: 2026/07/24 14:59:43 by nribakov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEBUG_H
# define DEBUG_H

# include "arena.h"
# include "types.h"

const char	*get_arena_name(t_arena *arena);
const char	*get_flag_name(uint32_t bit);
const char	*get_node_type_name(t_node_type type);
const char	*get_symbol_type_name(t_symbol_type type);
const char	*get_token_type_name(t_token_type type);
void		parse_debug_args(int argc, char **argv, t_ctx *c);
void		poison_stride(t_arena *arena);
void		print_arena(t_arena *arena);
void		print_flags(FILE *out, uint32_t flags);
void		print_token(FILE *out, t_ctx *c, t_token *token);
void		print_token_line(FILE *out, t_ctx *c, t_token *token);
void		print_symbol(t_ctx *c, t_symbol *symbol, uint64_t idx);
void		print_stack(t_ctx *c, t_parser_state *parse);
void		print_symbol_line(FILE *out, t_ctx *c, t_symbol *symbol, uint64_t idx);
void		print_tokens(t_ctx *c);
void		print_symbols(t_ctx *c, t_parser_state *parse);
void		print_node_line(FILE *out, t_ctx *c, t_node *node, uint64_t idx);
void		print_nodes(t_ctx *c);
void		print_lex_state(t_ctx *c, t_lexer_state *l);
void		print_char_info(unsigned char ch);
void		print_escaped_str(FILE *out, const char *str);

#endif
