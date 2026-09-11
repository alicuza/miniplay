/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nribakov <nribakov@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/05 18:41:57 by sancuta           #+#    #+#             */
/*   Updated: 2026/09/07 00:21:38 by nribakov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
# define PARSER_H

# include "types.h"

/* -------- LALR grammar constants (from shni_grammar_reduced.output) ------- */
# define YYPACT_NINF	-44
# define YYFINAL		5
# define YYLAST			67
# define NTERM_OFFSET	14

/* -------- parser/parse_input.c -------------------------------------------- */
t_parser_state	parse_input(t_ctx *c);

/* -------- parser/parse_token_flow.c --------------------------------------- */
bool			get_next_token(t_ctx *c, t_parser_state *p, t_lexer_state *l);
bool			get_lookahead(t_ctx *c, t_parser_state *p, t_lexer_state *l);
bool			handle_here_doc(t_ctx *c, t_parser_state *p);
void			report_parse_error(t_ctx *c, t_parser_state *p);

/* -------- parser/classify_token.c ----------------------------------------- */
t_symbol_type	classify_token(t_ctx *c, t_token *token);

/* -------- parser/yy_pact.c / parser/yy_table.c ---------------------------- */
int32_t			get_yypact(uint64_t idx);
int32_t			get_yydefact(uint64_t idx);
int32_t			get_yypgoto(uint64_t idx);
int32_t			get_yydefgoto(uint64_t idx);
int32_t			get_yytable(uint64_t idx);
int32_t			get_yycheck(uint64_t idx);

/* -------- parser/rule_dispatch.c ------------------------------------------ */
t_rule			get_rule(int32_t action);

/* -------- parser/stack_ops.c ---------------------------------------------- */
void			push_symbol(t_ctx *c, t_parser_state *p, t_symbol *src);
void			pop_symbols(t_ctx *c, t_parser_state *p, uint32_t len);
void			reduce_apply_goto(t_parser_state *p, t_rule *r);

/* -------- parser/shift_reduce.c ------------------------------------------- */
t_lalr_action	shift_reduce(t_ctx *c, t_parser_state *p);

/* -------- parser/reduce_helpers.c ----------------------------------------- */
uint64_t		reduce_compute_result(t_ctx *c, t_parser_state *p, t_rule *r,
					uint64_t *token_idx);
uint64_t		get_exec_root(t_ctx *c, t_parser_state *p, t_rule *r);
void			init_parser(t_ctx *c, t_parser_state *p, t_lexer_state *l);

/* -------- parser/reduce_*.c handlers -------------------------------------- */
uint64_t		reduce_from_term(t_ctx *c, t_parser_state *p, t_rule *r);
uint64_t		reduce_program(t_ctx *c, t_parser_state *p, t_rule *r);
uint64_t		reduce_list_append(t_ctx *c, t_parser_state *p, t_rule *r);
uint64_t		reduce_and_or_conditional(t_ctx *c, t_parser_state *p,
					t_rule *r);
uint64_t		reduce_pipeline_create(t_ctx *c, t_parser_state *p, t_rule *r);
uint64_t		reduce_pipeline_append(t_ctx *c, t_parser_state *p, t_rule *r);
uint64_t		reduce_simple_command_from_suffix(t_ctx *c, t_parser_state *p,
					t_rule *r);
uint64_t		reduce_simple_command(t_ctx *c, t_parser_state *p, t_rule *r);
uint64_t		reduce_cmd_suffix(t_ctx *c, t_parser_state *p, t_rule *r);
uint64_t		reduce_redir_append(t_ctx *c, t_parser_state *p, t_rule *r);
uint64_t		reduce_io_file(t_ctx *c, t_parser_state *p, t_rule *r);
uint64_t		reduce_io_here(t_ctx *c, t_parser_state *p, t_rule *r);
uint64_t		reduce_subshell_redirects(t_ctx *c, t_parser_state *p,
					t_rule *r);
uint64_t		reduce_compound_list(t_ctx *c, t_parser_state *p, t_rule *r);
uint64_t		reduce_subshell(t_ctx *c, t_parser_state *p, t_rule *r);

/* -------- parser/node_utils.c --------------------------------------------- */
uint64_t		alloc_node(t_ctx *c, t_node_type type);
t_node			*get_node_from_idx(t_ctx *c, uint64_t idx);
uint64_t		get_node_tail_idx(t_ctx *c, uint64_t head_idx);
uint64_t		append_node_to_tail(t_ctx *c, uint64_t head_idx,
					uint64_t new_idx);

/* -------- parser/quote_remove.c ------------------------------------------- */
char 			*quote_remove_inplace(char *s);

/* -------- parser/node_getters.c ------------------------------------------- */
t_symbol		*get_symbol_from_rhs(t_ctx *c, t_parser_state *p, t_rule *r,
					uint32_t rhs_pos);
t_token			*get_token_from_idx(t_ctx *c, uint64_t idx);
char			*get_token_body(t_ctx *c, t_token *token);
t_symbol		*get_symbol_from_top(t_ctx *c, uint32_t depth);
t_symbol		*get_symbol_from_idx(t_ctx *c, uint64_t idx);
#endif
