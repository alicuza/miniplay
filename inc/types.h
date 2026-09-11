/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   types.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nribakov <nribakov@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 08:07:58 by sancuta           #+#    #+#             */
/*   Updated: 2026/09/11 13:35:39 by sancuta          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TYPES_H
# define TYPES_H

# include "arena.h"

typedef struct s_env_content
{
	char	*key;
	char	*val;
}	t_env_content;

typedef struct s_env
{
	t_list	*vals;
}	t_env;

typedef enum e_arena_type
{
	AT_PROMPT,
	AT_STRING,
	AT_TOKENS,
	AT_STACK,
	AT_COMMAND,
	AT_FIELDS,
	AT_ARGV,
	AT_COUNT,
}	t_arena_type;

# ifdef DEBUG
typedef struct s_debug
{
	uint8_t	scope;				/* SCOPE_TOKENS | SCOPE_STACK | SCOPE_COMMAND */
	uint8_t	states;				/* DBG_LEXER | DBG_PARSER | DBG_HEREDOC */
	uint8_t	arenas;				/* DBG_ARENA_* mask */
	uint8_t	parser;				/* DBG_SHOW_* mask: which parser parts to print */
	bool	no_exec;
}	t_debug;
# endif

typedef struct s_ctx	t_ctx;

typedef char	*(*t_read)(t_ctx *c, bool is_continuation);

typedef struct s_ctx
{
	t_env	env;
	int		pipe_fd[2];
	int		io_fd[2];
	t_arena	arena[AT_COUNT];
	t_read	read_handler;
	char	*read_line;
	int		return_status;
	bool	is_interactive;
	bool	is_pipe;
	bool	should_exit;
	int		pid_to_wait;
# ifdef DEBUG
	t_debug	dbg;
# endif
}	t_ctx;

typedef struct s_slice
{
	uint64_t	pos;
	uint64_t	len;
}	t_slice;

typedef struct s_command_ctx
{
	char		*pathname;
	uint64_t	argc;
	char		**argv;
}	t_command_ctx;

typedef struct s_expand_state
{
	t_slice	field;
	uint8_t	flags;
}	t_expand_state;

typedef struct s_field_ref
{
	t_arena_type	arena;
	uint32_t		offset;
}	t_field_ref;

typedef union u_argv_slot
{
	t_field_ref	ref;
	char		*ptr;
}	t_argv_slot;

typedef int	(*t_command_function)(t_ctx *c, t_command_ctx *command_ctx);

typedef enum e_token_type
{
	TKN_NONE,
	TKN_EOF,
	TKN_WORD,
	TKN_OPERATOR,
}	t_token_type;

typedef enum e_symbol_type
{
	/* -------- eof token ------------------------------------------------------- */
	SYM_EOF = 0,
	/* -------- lexical tokens -------------------------------------------------- */
	SYM_WORD = 3,
	SYM_NEWLINE = 4,
	SYM_PIPE = 5,
	SYM_LESS = 6,
	SYM_GREAT = 7,
	SYM_DLESS = 8,
	SYM_DGREAT = 9,
	SYM_AND_IF = 10,
	SYM_OR_IF = 11,
	SYM_OPAR = 12,
	SYM_CPAR = 13,
	SYM_ACCEPT = 14,
	/* -------- entrypoint ------------------------------------------------------ */
	SYM_PROGRAM = 15,
	/* -------- complete_commands constructs ------------------------------------ */
	SYM_COMPLETE_COMMANDS = 16,
	/* -------- list constructs ------------------------------------------------- */
	SYM_AND_OR = 17,
	/* -------- pipeline constructs --------------------------------------------- */
	SYM_PIPELINE = 18,
	/* -------- command constructs ---------------------------------------------- */
	SYM_COMMAND = 19,
	SYM_SUBSHELL = 20,
	SYM_COMPOUND_LIST = 21,
	SYM_TERM = 22,							/* actually also a list construct */
	SYM_SIMPLE_COMMAND = 23,
	/* -------- simple_command constructs --------------------------------------- */
	SYM_CMD_NAME = 24,
	SYM_CMD_WORD = 25,
	SYM_CMD_PREFIX = 26,
	SYM_CMD_SUFFIX = 27,
	/* -------- redirection constructs ------------------------------------------ */
	SYM_REDIRECT_LIST = 28,
	SYM_IO_REDIRECT = 29,
	SYM_IO_FILE = 30,
	SYM_FILENAME = 31,
	SYM_IO_HERE = 32,
	SYM_HERE_END = 33,
	/* -------- separation contructs -------------------------------------------- */
	SYM_NEWLINE_LIST = 34,
	SYM_LINEBREAK = 35,
}	t_symbol_type;

typedef enum e_node_type
{
	NODE_DEFAULT,
	NODE_PIPELINE,
	NODE_COMMAND,
	NODE_ARG,
	NODE_REDIR,
}	t_node_type;

typedef struct s_token
{
	uint64_t		offset;			/* into AT_STRING */
	t_token_type	type;
	uint8_t			flags;			/* TKN_HAS_QUOTES | TKN_HAS_EXPANSION */
}	t_token;

typedef struct s_symbol
{
	uint64_t		node_idx;		/* current node in AT_COMMAND*/
	uint64_t		token_idx;		/* current token in AT_TOKENS */
	uint32_t		entry_state;
	t_symbol_type	type;
}	t_symbol;

typedef struct s_node_pipeline
{
	uint64_t	command_head_idx;
}	t_node_pipeline;

typedef struct s_node_command
{
	uint64_t	arg_head_idx;
	uint64_t	redir_head_idx;
}	t_node_command;

typedef struct s_node_arg
{
	uint64_t	arena_offset;
	uint8_t	flags;
}	t_node_arg;

typedef struct s_node_redir
{
	uint64_t	arena_offset;
	int32_t		fd;					// NOTE: only for heredocs?
	uint8_t	flags;
}	t_node_redir;

typedef union u_node_data
{
	t_node_pipeline	pipeline;
	t_node_command	command;
	t_node_arg		arg;
	t_node_redir	redir;
}	t_node_data;

/* tagged union */
typedef struct s_node
{
	t_node_data	data;
	uint64_t	next_idx;
	t_node_type	type;
	uint8_t		flags;				/* FLAG_AND_IF | FLAG_OR_IF | FLAG_SUBSHELL | REDIR_IN | REDIR_OUT | REDIR_HERE | REDIR_APPEND */
}	t_node;

typedef enum e_lalr_action
{
	LALR_SHIFT,
	LALR_REDUCE,
	LALR_ACCEPT,
	LALR_ERROR,
}	t_lalr_action;

typedef struct s_parser_state
{
	uint64_t		stack_idx;		/* current top symbol on the stack */
	uint64_t		token_idx;		/* current lookahead token in AT_TOKENS */
	uint64_t		exec_root_idx;	/* completed cmd root, awaiting exec */
	int32_t			state;			/* current state of the stack */
	t_symbol_type	lookahead_type;	/* classified type of the lookahead token */
	uint8_t			flags;			/* PARSE_SAVE_TOKENS | PARSE_HERE_BODY | PARSE_ERROR */
}	t_parser_state;

typedef struct s_lexer_state
{
	t_slice			token;			/* tracks the position and length of the current token being built */
	uint64_t		char_idx;		/* offset into the input string */
	t_token_type	type;
	uint8_t			flags;			/* TKN_HAS_EXPANSION | TKN_HAS_QUOTES | LEX_IS_BUILDING | LEX_NEW_INPUT */
}	t_lexer_state;

typedef struct s_rule	t_rule;

typedef uint64_t	(*t_reduce)(t_ctx *, t_parser_state *, t_rule *);

typedef struct s_rule
{
	t_reduce		handler;		/* pointer to handler function */
	uint32_t		rhs_len;		/* number of rhs symbols in rule */
	t_symbol_type	lhs_type;		/* type of lhs in rule */
}	t_rule;
#endif
