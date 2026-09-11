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

# define DBG_CAT_COUNT 4

/* one "name -> bit" mapping in a --flag= list */
typedef struct s_dbg_flag
{
	const char	*name;
	uint8_t		bit;
}	t_dbg_flag;

/* one --flag= category: its prefixes, the mask it fills, and its table */
typedef struct s_dbg_cat
{
	const char			*prefix;
	const char			*alias;
	uint8_t				*mask;
	const t_dbg_flag	*flags;
	uint8_t				all;
	uint8_t				def;
}	t_dbg_cat;

/* one shift/reduce step, rendered as a single trace line */
typedef struct s_debug_step
{
	t_lalr_action	kind;
	int32_t			rule;	/* reduce rule number (0 for shift/accept) */
	int32_t			from;	/* state the action was decided in */
}	t_debug_step;

/* -------- debug/debug_main.c ---------------------------------------------- */
void		debug_print_read_line(t_ctx *c);
void		debug_print_after_parse(t_ctx *c, t_parser_state *parse);

/* -------- debug/debug_utils.c --------------------------------------------- */
void		print_char_info(unsigned char ch);
size_t		escape_into_buf(char *dst, size_t size, const char *src,
				size_t len);
void		print_escaped_str(FILE *out, const char *str);
void		print_escaped_strn(FILE *out, const char *str, size_t len);
void		parse_flag_list(const char *spec, uint8_t *mask,
				const t_dbg_flag *flags, uint8_t all);
void		parse_debug_args(int argc, char **argv, t_ctx *c);

/* -------- debug/debug_names.c --------------------------------------------- */
const char	*get_token_type_name(t_token_type type);
const char	*get_symbol_type_name(t_symbol_type type);
const char	*get_node_type_name(t_node_type type);
const char	*get_node_flag_name(uint32_t bit);
const char	*get_lex_flag_name(uint32_t bit);
const char	*get_parse_flag_name(uint32_t bit);

/* -------- debug/debug_arena.c --------------------------------------------- */
void		poison_stride(t_arena *arena);
const char	*get_arena_name(t_arena *arena);
void		print_arena(t_arena *arena);

/* -------- debug/debug_lex.c ----------------------------------------------- */
void		print_lex_rule(t_ctx *c, int n);
void		print_flags(FILE *out, uint32_t flags);
void		print_lex_state(t_ctx *c, t_lexer_state *l);
void		print_token(FILE *out, t_ctx *c, t_token *token);
void		print_token_line(FILE *out, t_ctx *c, t_token *token);

/* -------- debug/debug_parse.c --------------------------------------------- */
void		print_symbol(FILE *out, t_ctx *c, t_symbol *symbol, uint64_t idx);
void		print_tokens(FILE *out, t_ctx *c);
void		print_node_flags(FILE *out, uint8_t flags);
void		print_node_line(FILE *out, t_ctx *c, t_node *node, uint64_t idx);
void		print_stack(FILE *out, t_ctx *c, t_parser_state *parse);
void		print_nodes(FILE *out, t_ctx *c);
void		print_step(t_ctx *c, t_parser_state *parse, t_debug_step step);
void		log_rhs_symbols(t_ctx *c, t_parser_state *parse, t_rule *rule);
void		debug_parse_header(t_parser_state *parse);
void		debug_parse_arenas(t_ctx *c);
void		debug_parse_action(t_ctx *c, t_parser_state *parse,
				t_lalr_action action);

/* -------- debug/debug_heredoc.c ------------------------------------------- */
void		print_here_reading(t_ctx *c, t_parser_state *parse);
void		print_here_line(t_ctx *c, t_lexer_state *lex, uint64_t line,
				uint64_t len);
void		print_here_stored(t_parser_state *parse, uint64_t token_idx);
void		print_here_saving(void);
void		print_here_replay(uint64_t idx);
#endif
