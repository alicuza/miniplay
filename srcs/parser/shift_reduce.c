/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shift_reduce.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sancuta <sancuta@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 12:41:20 by sancuta           #+#    #+#             */
/*   Updated: 2026/09/11 12:41:27 by sancuta          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_lalr_action	reduce(t_ctx *c, t_parser_state *parse, int32_t action)
{
	t_rule		rule;
	t_symbol	sym;
#ifdef DEBUG
	int32_t		from;

	from = parse->state;
#endif
	rule = get_rule(action);
	sym.node_idx = reduce_compute_result(c, parse, &rule, &sym.token_idx);
	sym.type = rule.lhs_type;
	parse->exec_root_idx = get_exec_root(c, parse, &rule);
	pop_symbols(c, parse, rule.rhs_len);
	reduce_apply_goto(parse, &rule);
#ifdef DEBUG
	if ((c->dbg.states & DBG_PARSER) && (c->dbg.parser & DBG_SHOW_LINKS))
	{
		if (sym.node_idx)
		{
			fprintf(stderr, "[lhs] ");
			print_node_line(stderr, c, get_node_from_idx(c, sym.node_idx),
				sym.node_idx);
		}
	}
#endif
	push_symbol(c, parse, &sym);
#ifdef DEBUG
	print_step(c, parse, (t_debug_step){LALR_REDUCE, action, from});
#endif
	if (parse->state == YYFINAL)
	{
#ifdef DEBUG
		print_step(c, parse, (t_debug_step){LALR_ACCEPT, 0, from});
#endif
		return (LALR_ACCEPT);
	}
	return (LALR_REDUCE);
}

static t_lalr_action	reduce_or_error(t_ctx *c, t_parser_state *parse,
		int32_t action)
{
	if (!action)
		return (LALR_ERROR);
	return (reduce(c, parse, action));
}

static t_lalr_action	shift(t_ctx *c, t_parser_state *parse, int32_t action)
{
	t_symbol	sym;
#ifdef DEBUG
	int32_t		from;

	from = parse->state;
#endif
	parse->state = action;
#ifdef DEBUG
	print_step(c, parse, (t_debug_step){LALR_SHIFT, 0, from});
#endif
	if (parse->state == YYFINAL)
	{
#ifdef DEBUG
		print_step(c, parse, (t_debug_step){LALR_ACCEPT, 0, from});
#endif
		return (LALR_ACCEPT);
	}
	sym.type = parse->lookahead_type;
	sym.token_idx = parse->token_idx;
	sym.node_idx = 0;
	push_symbol(c, parse, &sym);
	return (LALR_SHIFT);
}

t_lalr_action	shift_reduce(t_ctx *c, t_parser_state *parse)
{
	int32_t		action;
	int32_t		index;

	index = get_yypact(parse->state);
	if (index == YYPACT_NINF)
		return (reduce_or_error(c, parse, get_yydefact(parse->state)));
	index += parse->lookahead_type;
	if (index < 0 || index > YYLAST
		|| get_yycheck(index) != (int32_t)parse->lookahead_type)
		return (reduce_or_error(c, parse, get_yydefact(parse->state)));
	action = get_yytable(index);
	if (action <= 0)
		return (reduce_or_error(c, parse, -action));
	return (shift(c, parse, action));
}
