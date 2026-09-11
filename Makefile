# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nribakov <nribakov@student.42vienna.com    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/05/22 21:29:56 by sancuta           #+#    #+#              #
#    Updated: 2026/09/07 05:43:42 by nribakov         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# ---- project ------------------------------------------------------------- #
NAME       = minishell

# ---- compiler / linker --------------------------------------------------- #
CC         = cc
CFLAGS     = -Wall -Wextra -Werror -MMD -MP
ARENA_SIZE ?= 64
CPPFLAGS   = -I inc -I $(LIBFT_DIR) -I $(LIBFT_DIR)/arena -I $(LIBFT_DIR)/gnl -D ARENA_SIZE=$(ARENA_SIZE)
LDFLAGS    =
LDLIBS     = -lreadline

# ---- build directories --------------------------------------------------- #
RELEASE_DIR = build/release
DEBUG_DIR   = build/debug

# ---- libraries ----------------------------------------------------------- #
LIBFT_DIR     = libs/libft
LIBFT_RELEASE = $(LIBFT_DIR)/libft.a
LIBFT_DEBUG   = $(DEBUG_DIR)/libft.a

# ---- sources ------------------------------------------------------------- #
SRCS        = \
			main.c \
			prompt.c \
			input.c \
			ft_close_fd.c \
			cleanup.c \
			expansions/expansion.c \
			expansions/expand_field.c \
			expansions/expand_quote.c \
			expansions/expand_var.c \
			expansions/expand_helpers.c \
			execute/execute_list.c \
			execute/execute_pipeline.c \
			execute/execute_simple_command.c \
			execute/execute_non_builtin.c \
			execute/get_pathname.c \
			execute/process_redirection.c \
			execute/ft_split_with_empty.c \
			execute/command_search_and_execution.c \
			execute/build_command.c \
			execute/wait_return_status.c \
			parser/parse_input.c \
			parser/parse_token_flow.c \
			parser/classify_token.c \
			parser/yy_pact.c \
			parser/yy_table.c \
			parser/rule_dispatch.c \
			parser/stack_ops.c \
			parser/shift_reduce.c \
			parser/reduce_helpers.c \
			parser/reduce_from_term.c \
			parser/reduce_pipeline.c \
			parser/reduce_command.c \
			parser/reduce_redir.c \
			parser/reduce_compound.c \
			parser/node_utils.c \
			parser/quote_remove.c \
			parser/node_getters.c \
			lexer/lookahead.c \
			lexer/lookahead_rules1.c \
			lexer/lookahead_rules2.c \
			lexer/lex_tokens.c \
			lexer/lex_heredoc.c \
			lexer/lex_utils.c \
			lexer/here_body_read.c \
			lexer/here_read_line.c \
			lexer/here_write_line.c \
			lexer/pair_utils.c \
			lexer/expand_utils.c \
			environment/env_add.c \
			environment/env_update.c \
			environment/env_get.c \
			environment/env_delete.c \
			environment/free_env.c \
			environment/init_env.c \
			environment/add_env_defaults.c \
			environment/env_to_envp.c \
			error_handling/error_handling.c \
			error_handling/msh_error.c \
			builtin/execute_builtin.c \
			builtin/execute_builtin_in_subshell.c \
			builtin/env.c \
			builtin/ft_split_key_value.c \
			builtin/pwd.c \
			builtin/builtin_exit.c \
			builtin/cd.c \
			builtin/get_path_canonical_form.c \
			builtin/builtin_export.c \
			builtin/unset.c \
			builtin/echo.c \
			utils/str_utils.c \
			utils/var_utils.c \
			signals/signal_setup.c \
			signals/signal_helpers.c

DEBUG_SRCS  = \
			debug/debug_main.c \
			debug/debug_utils.c \
			debug/debug_names.c \
			debug/debug_arena.c \
			debug/debug_lex.c \
			debug/debug_parse.c \
			debug/debug_heredoc.c

RELEASE_OBJS = $(addprefix $(RELEASE_DIR)/, $(SRCS:.c=.o))
DEBUG_OBJS   = $(addprefix $(DEBUG_DIR)/, $(SRCS:.c=.o)) \
               $(addprefix $(DEBUG_DIR)/, $(DEBUG_SRCS:.c=.o))

# ---- source lookup ------------------------------------------------------- #
vpath %.c srcs

# ---- build flags --------------------------------------------------------- #
RELEASE_FLAGS = -O2
DEBUG_FLAGS   = -O0 -g3 -DDEBUG

# ---- targets ------------------------------------------------------------- #
all: $(NAME)

$(NAME): $(RELEASE_OBJS) $(LIBFT_RELEASE)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

debug: $(NAME)-debug

$(NAME)-debug: $(DEBUG_DIR)/$(NAME)
	ln -sf $(DEBUG_DIR)/$(NAME) $@

$(DEBUG_DIR)/$(NAME): $(DEBUG_OBJS) $(LIBFT_DEBUG)
	@mkdir -p $(@D)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

# ---- object compilation -------------------------------------------------- #
$(RELEASE_DIR)/%.o: BUILD_FLAGS = $(RELEASE_FLAGS)
$(DEBUG_DIR)/%.o:   BUILD_FLAGS = $(DEBUG_FLAGS)

$(RELEASE_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(BUILD_FLAGS) -c $< -o $@

$(DEBUG_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(BUILD_FLAGS) -c $< -o $@

# ---- libft --------------------------------------------------------------- #
$(LIBFT_RELEASE): FORCE
	$(MAKE) -C $(LIBFT_DIR)

$(LIBFT_DEBUG): FORCE
	$(MAKE) -C $(LIBFT_DIR) debug
	cp -u $(LIBFT_RELEASE) $@

FORCE:

# ---- utility targets ----------------------------------------------------- #
run: $(NAME)
	./$(NAME)

run-debug: $(NAME)-debug
	./$(NAME)-debug

doc:
	printf '%s\n' \
	  'OUTPUT_DIRECTORY = docs' \
	  'GENERATE_MAN = YES' \
	  'MAN_OUTPUT = man' \
	  'INPUT = inc srcs debug_srcs' \
	  'EXTRACT_ALL = YES' \
	  'OPTIMIZE_OUTPUT_FOR_C = YES' \
	  > docs/Doxyfile
	doxygen docs/Doxyfile

compile_flags.txt:
	@echo setup $@
	@echo -n > $@
	@for flag in $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $(LDLIBS); do \
		echo $$flag >> $@ ; \
	done

test: $(NAME) $(NAME)-debug
	./test/runner.sh

clean:
	$(MAKE) -C $(LIBFT_DIR) clean
	rm -rf build docs/Doxyfile

fclean: clean
	$(MAKE) -C $(LIBFT_DIR) fclean
	rm -f $(NAME) $(NAME)-debug
	rm -rf docs
	rm -f compile_flags
	rm -rf log

re: 
	$(MAKE) fclean
	$(MAKE) all

# ---- phony targets ------------------------------------------------------- #
.PHONY: all debug run run-debug doc test clean fclean re FORCE

# ---- dependencies -------------------------------------------------------- #
-include $(RELEASE_OBJS:.o=.d)
-include $(DEBUG_OBJS:.o=.d)
