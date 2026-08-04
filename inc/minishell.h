/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nribakov <nribakov@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 21:48:28 by sancuta           #+#    #+#             */
/*   Updated: 2026/08/03 12:38:02 by sancuta          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>             // printf, perror
# include <readline/readline.h> // readline, rl_clear_history, rl_on_new_line,
# include <string.h>            // strerror
								// rl_replace_line, rl_redisplay
# include <readline/history.h>  // add_history
# include <stdlib.h>            // malloc, free, exit, getenv
# include <unistd.h>            // access, write, read, fork, dup, dup2, pipe,
								// getcwd, chdir, unlink, execve,
								// isatty, ttyname, ttyslot
# include <fcntl.h>             // open, close
# include <signal.h>            // signal, sigaction, sigemptyset, sigaddset,
								// kill
# include <dirent.h>            // opendir, readdir, closedir
# include <sys/ioctl.h>         // ioctl
# include <sys/stat.h>          // stat, lstat, fstat
# include <sys/wait.h>          // wait, waitpid, wait3, wait4
# include <termcap.h>           // tgetent, tgetflag, tgetnum, tgetstr, tgoto,
# include <termios.h>           // tcgetattr, tcsetattr
								// tputs
# include "arena.h"
# include "libft.h"
# include "types.h"
# include <errno.h> // errno

# ifdef DEBUG
#  include "debug.h"
# endif

# ifndef ARENA_SIZE
#  define ARENA_SIZE 64
# endif

# define SHELLNAME "shni"

/* -------- with_cwd -------------------------------------------------------- */
# define WITH_CWD true
# define NO_CWD false

/* -------- prompt type ----------------------------------------------------- */
# define INPUT_DEFAULT 0
# define INPUT_CONTINUATION 1

/* -------- operators ------------------------------------------------------- */
# define NL "\n"
# define PIPE "|"
# define LESS "<"
# define GREAT ">"
# define DLESS "<<"
# define DGREAT ">>"
# define AND_IF "&&"
# define OR_IF "||"
# define OPAR "("
# define CPAR ")"

/* -------- sets ------------------------------------------------------------ */
# define OPERATOR_SET "<>&|()\n"
# define BLANK_SET " \t"
# define QUOTE_SET "\"'"
# define SPECIAL_PARAM_SET "?"

/* -------- lexer flags ----------------------------------------------------- */
# define TKN_HAS_QUOTES			0x01
# define TKN_HAS_EXPANSION		0x02
# define TKN_IS_HERE_BODY		0x04
# define LEX_IS_BUILDING		0x08
# define LEX_AT_EOI				0x10

/* -------- parser flags ---------------------------------------------------- */
# define PARSE_DONE				0x01
# define PARSE_SAVE_TOKENS		0x02
# define PARSE_HAS_SAVED_TOKENS	0x04
# define PARSE_HERE_BODY		0x10
# define PARSE_ERROR			0x20
# define PARSE_LOOKAHEAD_EOF	0x40

/* -------- node flags ------------------------------------------------------ */
# define FLAG_AND_IF 0x01
# define FLAG_OR_IF 0x02
# define FLAG_SUBSHELL 0x04
# define REDIR_IN 0x08
# define REDIR_OUT 0x10
# define REDIR_HERE 0x20
# define REDIR_APPEND 0x40
# define REDIR_HAS_QUOTES 0x80

# ifdef DEBUG
/* -------- test scope flags ------------------------------------------------ */
#  define SCOPE_TOKENS 0x01
#  define SCOPE_STACK 0x02
#  define SCOPE_COMMAND 0x04

/* -------- debug state flags (--states=) ----------------------------------- */
#  define DBG_LEXER 0x01
#  define DBG_PARSER 0x02
#  define DBG_HEREDOC 0x04
#  define DBG_ALL_STATES 0x07

/* -------- debug arena flags (--arenas=) ----------------------------------- */
#  define DBG_ARENA_PROMPT 0x01
#  define DBG_ARENA_STRING 0x02
#  define DBG_ARENA_TOKENS 0x04
#  define DBG_ARENA_STACK 0x08
#  define DBG_ARENA_COMMAND 0x10
#  define DBG_ARENA_ALL 0x1f
#endif

/* -------- grammar constants ----------------------------------------------- */
# define NO_TOKEN 0
# define MAX_RHS_LEN 4
# define RULE_COUNT 46

/* -------- prompt.c -------------------------------------------------------- */
char			*get_prompt(t_ctx *c, bool with_cwd);

/* -------- input.c --------------------------------------------------------- */
char			*get_user_input(t_ctx *c, bool is_continuation);

/* -------- lookahead.c ----------------------------------------------------- */
bool			get_next_token(t_ctx *c, t_parser_state *p, t_lexer_state *l);
bool			lex_token(t_ctx *c, t_lexer_state *lex);

/* -------- pair_utils.c ---------------------------------------------------- */
char			matching_close(char open);
bool			find_matched_pair(t_ctx *c, t_lexer_state *lex);

/* -------- token_transform_utils.c ----------------------------------------- */
uint64_t		get_idx_from_offset(t_arena *arena, uint64_t offset);
uint64_t		get_offset_from_idx(t_arena *arena, uint64_t idx);
void			*get_ptr_from_offset(t_arena *arena, uint64_t offset);
void			*get_ptr_from_idx(t_arena *arena, uint64_t idx);
char			*get_token_content(t_ctx *c, t_token *token);

/* -------- lex_tokens.c ---------------------------------------------------- */
void			start_lex_token(t_lexer_state *lex, t_token_type type);
void			delimit_lex_token(t_ctx *c, t_lexer_state *lex);
uint64_t		grow_lex_token(t_lexer_state *lex, uint64_t len);

/* -------- lex_heredoc.c --------------------------------------------------- */
uint64_t		get_here_doc(t_ctx *c, t_lexer_state *l, t_here_state *h);
bool			is_delim_line(t_ctx *c, t_lexer_state *l, t_here_state *);
uint64_t		delimit_lex_here(t_ctx *c, t_here_state *h);
bool			handle_here_body(t_ctx *c, t_parser_state *p, t_lexer_state *l);
bool			handle_saved_tokens(t_ctx *c, t_parser_state *parse);

/* -------- lex_utils.c ----------------------------------------------------- */
uint64_t		consume_char(t_lexer_state *lex, uint64_t len);
t_slice			save_lex_token_slice(t_lexer_state *lex);
void			restore_lex_token_slice(t_lexer_state *lex, t_slice len);

/* -------- shift_reduce.c ----------------------------------------------------- */
t_lalr_action	shift_reduce(t_ctx *c, t_parser_state *parse, t_lexer_state *lex);

/* -------- string_utils.c -------------------------------------------------- */
const char		**get_operator_strs(void);
bool			is_char_in_set(char c, const char *set);
bool			is_str_in_set(char *c, const char **set);
bool			is_name_start(char c);
bool			is_name_body(char c);

/* -------- expand_utils.c -------------------------------------------------- */
bool			is_expansion_start(char *buffer, uint64_t idx);
uint64_t		get_expansion_len(char *expansion);

/* -------- env_add.c ---------------------------------------------------- */
int				env_add(t_env *env, char *key, char *value);

/* -------- env_update.c ---------------------------------------------------- */
int				env_update(t_env *env, char *key, char *value);
int				env_update_with_copy(t_env *env, char *key, char *value);

/* -------- env_get.c ------------------------------------------------------- */
char			*env_get(t_env *env, char *key);

/* -------- env_delete.c ------------------------------------------------------- */
void			env_delete(t_env *env, char *key);

/* -------- free_env.c ------------------------------------------------------- */
void			free_env_content(void *content_void_p);
void			free_env(t_env *env);

/* -------- init_env.c ---------------------------------------------------- */
int				init_env(t_env *env, char **envp);

/* -------- add_env_defaults.c ---------------------------------------------------- */
int				add_env_defaults(t_env *env);

/* -------- env_to_envp.c ---------------------------------------------------- */
char			**env_to_envp(t_env *env);

/* -------- ft_split_key_value.c -------------------------------------------- */
char			**ft_split_key_value(const char *s, char c);

/* -------- token_processor.c ----------------------------------------------- */
int				process_token(t_ctx *c, t_token *token);
void			exec_list(t_ctx *c, uint64_t head_idx);

/* -------- execute_non_builtin.c ----------------------------------------------- */
int				execute_non_builtin(t_ctx *c, t_command_ctx *cmd_ctx);

/* -------- get_pathname.c ----------------------------------------------- */
int				get_pathname(t_ctx *c, t_command_ctx *cmd_ctx);

/* -------- ft_split_with_empty.c ----------------------------------------------- */
char			**ft_split_with_empty(char const *s, char c);

/* -------- env.c ---------------------------------------------------- */
int				env(t_ctx *c, t_command_ctx *command_ctx);

/* -------- pwd.c ---------------------------------------------------- */
int				pwd(t_ctx *c, t_command_ctx *command_ctx);
char			*get_pwd(t_ctx *c);

/* -------- parse_input.c --------------------------------------------------- */
void			parse_input(t_ctx *c, t_parser_state *parse);
void			finalize_parse(t_ctx *c, t_parser_state *parse);
void			reset_parser(t_ctx *c, t_parser_state *parse);

/* -------- classify_token.c -------------------------------------------------- */
t_symbol_type	classify_token(t_ctx *c, t_token *token);

/* -------- builtin_exit.c ------------------------------ */
int				builtin_exit(t_ctx *c, t_command_ctx *command_ctx);

int				exit_mem_issue(void);

/* -------- cd.c ------------------------------ */
int				cd(t_ctx *c, t_command_ctx *command_ctx);

/* -------- get_path_canonical_form.c ------------------------------ */
char			*get_path_canonical_form(char *curpath, size_t len);

/* -------- builtin_export.c ------------------------------ */
int				builtin_export(t_ctx *c, t_command_ctx *command_ctx);

/* -------- unset.c ------------------------------ */
int				unset(t_ctx *c, t_command_ctx *command_ctx);

/* -------- echi.c ------------------------------ */
int				echo(t_ctx *c, t_command_ctx *command_ctx);

#endif
