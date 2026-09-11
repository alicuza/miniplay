#include "minishell.h"

static void	start_here_body(t_parser_state *parse)
{
	parse->flags &= ~PARSE_SAVE_TOKENS;
	parse->flags |= PARSE_HAS_SAVED_TOKENS | PARSE_HERE_BODY;
}

bool	get_next_token(t_ctx *c, t_parser_state *parse, t_lexer_state *lex)
{
	while (true)
	{
		if (parse->flags & PARSE_HERE_BODY)
		{
			handle_here_body(c, parse, lex);
			if (lex->flags & LEX_INTERRUPTED)
				return (false);
		}
		if (parse->flags & PARSE_HAS_SAVED_TOKENS)
		{
			handle_saved_tokens(c, parse);
			return (true);
		}
		if (lex->flags & LEX_AT_EOI)
		{
			ft_memset(lex, 0, sizeof(t_lexer_state));
			return (false);
		}
		if (lex_token(c, lex))
		{
			++parse->token_idx;
			return (true);
		}
	}
}

void	report_parse_error(t_ctx *c, t_parser_state *parse)
{
	t_token	*token;
	char	*body;

	parse->flags |= PARSE_ERROR;
	token = get_token_from_idx(c, parse->token_idx);
	body = get_token_body(c, token);
	if (parse->lookahead_type == SYM_EOF)
		msh_error(NULL, NULL, "syntax error near unexpected token"
			" 'end of file'");
	else if (parse->lookahead_type == SYM_NEWLINE)
		msh_error(NULL, NULL, "syntax error near unexpected token"
			" 'newline'");
	else
	{
		ft_putstr_fd(SHELLNAME": syntax error near unexpected token '",
			STDERR_FILENO);
		ft_putstr_fd(body, STDERR_FILENO);
		ft_putendl_fd("'", STDERR_FILENO);
	}
}

bool	get_lookahead(t_ctx *c, t_parser_state *parse, t_lexer_state *lex)
{
	t_token	*token;

	while (true)
	{
		if (get_next_token(c, parse, lex))
		{
			token = get_token_from_idx(c, parse->token_idx);
			parse->lookahead_type = classify_token(c, token);
			parse->flags |= PARSE_HAS_LOOKAHEAD;
			return (true);
		}
		if ((lex->flags & LEX_INTERRUPTED)
			|| !(parse->flags & PARSE_SAVE_TOKENS))
			return (false);
		start_here_body(parse);
	}
}

bool	handle_here_doc(t_ctx *c, t_parser_state *parse)
{
	t_token	*cur;
	char	*body;

	cur = get_token_from_idx(c, parse->token_idx);
	body = get_token_body(c, cur);
	if (*body == '\n')
		start_here_body(parse);
	return (true);
}
