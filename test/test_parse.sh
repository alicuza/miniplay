. test/helpers.sh

# ------------------------------------------------------------------------------
# Parser token tests
# ------------------------------------------------------------------------------
#
# Each test sends one input through the debug binary and compares what comes
# back against an expected block.  A block is one or more sections, each
# introduced by a header keyword naming what it pins:
#
#   TOKENS   the lexer's token stream (--scope=tokens)
#   TRACE    every shift/reduce step — stack | lookahead | action — for the
#            whole parse, including the EOF finalization steps after the last
#            newline.  Pins parser-table behaviour that COMMAND cannot see,
#            e.g. which lookahead forced which reduction.  (--scope=trace)
#   COMMAND  the node tree the reductions built (--scope=command)
#   ERROR    stderr lines that start with "shni: "
#   STATUS   the exit status
#   STACK    the symbol stack at the end of parse_input(): symbols top-to-
#            bottom, then the current lookahead and last parser action.
#            (--scope=stack)
#
# Most tests only need TOKENS since the token stream alone pins the lexer.
# Combine sections when the same input is worth checking from multiple angles —
# see test_cmd_bare for TOKENS + TRACE + COMMAND on one input.
#
# Dimensions used in per-test annotations:
#
#   CMD     command name form(s)                bare | abs | rel
#
#   ARGC    argument count per command          0 | 1 | N
#
#   REDIR   kind                                in | heredoc | out | append
#           + properties                        with_space | no_space | no_escape
#
#   EXPAND  type                                question | var
#           + position                          bare | suffix | enclosed
#
#   QUOTES  type                                double | single | unclosed
#           + position                          bare | prefix
#
#   OP      pipeline/chain operator(s)          pipe | and_if | or_if
#
#   OTHER   other test attributes               comment
#
# Within a brace, `|` separates sub-attributes of a single entity (e.g.
# REDIR={out|no_space} means kind=out, properties=no_space).
#
# Commas within braces separate multiple entities of the same dimension
# (CMD, ARGC, REDIR, OP) — e.g. CMD={bare, bare},
# REDIR={in|with_space, out|with_space}.
#
# ------------------------------------------------------------------------------

# ----- Command forms ----------------------------------------------------------

# CMD={bare}  ARGC={1}  REDIR={}  EXPAND={}  QUOTES={}  OP={}
test_cmd_bare()
{
	local input expected
	input="$(cat <<- \eof
		ls
		eof
	)"
	expected="$(cat <<- \eof
		TOKENS
			TKN_WORD(ls)
			TKN_OPERATOR(\n)
		TRACE
			[state 0] [la] SYM_WORD(ls) [reduce 45] [goto 4]
			[state 4] [la] SYM_WORD(ls) [shift 7]
			[state 7] [la] SYM_NEWLINE(\n) [reduce 24] [goto 19]
			[state 19] [la] SYM_NEWLINE(\n) [reduce 23] [goto 18]
			[state 18] [la] SYM_NEWLINE(\n) [reduce 11] [goto 16]
			[state 16] [la] SYM_NEWLINE(\n) [reduce 9] [goto 15]
			[state 15] [la] SYM_NEWLINE(\n) [reduce 6] [goto 14]
			[state 14] [la] SYM_NEWLINE(\n) [reduce 5] [goto 13]
			[state 13] [la] SYM_NEWLINE(\n) [shift 1]
			[state 1] [la] SYM_EOF [reduce 42] [goto 32]
			[state 32] [la] SYM_EOF [reduce 44] [goto 33]
			[state 33] [la] SYM_EOF [reduce 2] [goto 2]
			[state 2] [la] SYM_EOF [shift 5]
			[state 2] [la] SYM_EOF [accept]
		COMMAND
			[id 1] NODE_ARG(ls) [next 0]
			[id 2] NODE_COMMAND [next 0] [arg_head 1] [redir_head 0]
			[id 3] NODE_PIPELINE [next 0] [command_head 2]
		eof
	)"
	assert_shell "$input" "$expected"
}

# CMD={abs}  ARGC={1}  REDIR={}  EXPAND={}  QUOTES={}  OP={}
test_cmd_abs()
{
	local input expected
	input="$(cat <<- \eof
		/bin/pwd
		eof
	)"
	expected="$(cat <<- \eof
		TOKENS
			TKN_WORD(/bin/pwd)
			TKN_OPERATOR(\n)
		eof
	)"
	assert_shell "$input" "$expected"
}

# CMD={rel}  ARGC={1}  REDIR={}  EXPAND={}  QUOTES={}  OP={}
test_cmd_rel()
{
	local input expected
	input="$(cat <<- \eof
		./minishell
		eof
	)"
	expected="$(cat <<- \eof
		TOKENS
			TKN_WORD(./minishell)
			TKN_OPERATOR(\n)
		eof
	)"
	assert_shell "$input" "$expected"
}

# ----- Arguments --------------------------------------------------------------

# CMD={bare}  ARGC={3}  REDIR={}  EXPAND={}  QUOTES={}  OP={}
test_args_bare()
{
	local input expected
	input="$(cat <<- \eof
		echo hello world
		eof
	)"
	expected="$(cat <<- \eof
		TOKENS
			TKN_WORD(echo)
			TKN_WORD(hello)
			TKN_WORD(world)
			TKN_OPERATOR(\n)
		eof
	)"
	assert_shell "$input" "$expected"
}

# CMD={abs}  ARGC={2}  REDIR={}  EXPAND={}  QUOTES={}  OP={}
test_args_abs()
{
	local input expected
	input="$(cat <<- \eof
		/bin/echo hello
		eof
	)"
	expected="$(cat <<- \eof
		TOKENS
			TKN_WORD(/bin/echo)
			TKN_WORD(hello)
			TKN_OPERATOR(\n)
		eof
	)"
	assert_shell "$input" "$expected"
}

# CMD={rel}  ARGC={3}  REDIR={}  EXPAND={}  QUOTES={}  OP={}
test_args_rel()
{
	local input expected
	input="$(cat <<- \eof
		./script.sh -a foo
		eof
	)"
	expected="$(cat <<- \eof
		TOKENS
			TKN_WORD(./script.sh)
			TKN_WORD(-a)
			TKN_WORD(foo)
			TKN_OPERATOR(\n)
		eof
	)"
	assert_shell "$input" "$expected"
}

# ----- Redirects -------------------------------------------------------------

# CMD={bare}  ARGC={1}  REDIR={in|with_space}  EXPAND={}  QUOTES={}  OP={}
test_redir_input()
{
	local input expected
	input="$(cat <<- \eof
		cat < file
		eof
	)"
	expected="$(cat <<- \eof
		TOKENS
			TKN_WORD(cat)
			TKN_OPERATOR(<)
			TKN_WORD(file)
			TKN_OPERATOR(\n)
		eof
	)"
	assert_shell "$input" "$expected"
}

# CMD={bare}  ARGC={1}  REDIR={heredoc|with_space|no_escape}  EXPAND={}  QUOTES={}  OP={}
test_redir_heredoc()
{
	local input expected
	input="$(cat <<- \eof
		cat << EOF
			hello
			EOF
		eof
	)"
	expected="$(cat <<- \eof
		TOKENS
			TKN_WORD(cat)
			TKN_OPERATOR(<<)
			TKN_WORD(EOF)
			TKN_OPERATOR(\n)
		eof
	)"
	assert_shell "$input" "$expected"
}

# CMD={bare}  ARGC={1}  REDIR={out|with_space}  EXPAND={}  QUOTES={}  OP={}
test_redir_output()
{
	local input expected
	input="$(cat <<- \eof
		echo > file
		eof
	)"
	expected="$(cat <<- \eof
		TOKENS
			TKN_WORD(echo)
			TKN_OPERATOR(>)
			TKN_WORD(file)
			TKN_OPERATOR(\n)
		eof
	)"
	assert_shell "$input" "$expected"
}

# CMD={bare}  ARGC={1}  REDIR={append|with_space}  EXPAND={}  QUOTES={}  OP={}
test_redir_append()
{
	local input expected
	input="$(cat <<- \eof
		echo >> file
		eof
	)"
	expected="$(cat <<- \eof
		TOKENS
			TKN_WORD(echo)
			TKN_OPERATOR(>>)
			TKN_WORD(file)
			TKN_OPERATOR(\n)
		eof
	)"
	assert_shell "$input" "$expected"
}

# CMD={bare}  ARGC={1}  REDIR={in|with_space, out|with_space}  EXPAND={}  QUOTES={}  OP={}
test_redir_multiple()
{
	local input expected
	input="$(cat <<- \eof
		cat < in > out
		eof
	)"
	expected="$(cat <<- \eof
		TOKENS
			TKN_WORD(cat)
			TKN_OPERATOR(<)
			TKN_WORD(in)
			TKN_OPERATOR(>)
			TKN_WORD(out)
			TKN_OPERATOR(\n)
		eof
	)"
	assert_shell "$input" "$expected"
}

# CMD={bare}  ARGC={1}  REDIR={out|no_space}  EXPAND={}  QUOTES={}  OP={}
test_redir_adjacent()
{
	local input expected
	input="$(cat <<- \eof
		echo>file
		eof
	)"
	expected="$(cat <<- \eof
		TOKENS
			TKN_WORD(echo)
			TKN_OPERATOR(>)
			TKN_WORD(file)
			TKN_OPERATOR(\n)
		eof
	)"
	assert_shell "$input" "$expected"
}

# ----- Operators --------------------------------------------------------------

# CMD={bare, bare}  ARGC={1, 1}  REDIR={}  EXPAND={}  QUOTES={}  OP={pipe}
test_op_pipe()
{
	local input expected
	input="$(cat <<- \eof
		ls | wc
		eof
	)"
	expected="$(cat <<- \eof
		TOKENS
			TKN_WORD(ls)
			TKN_OPERATOR(|)
			TKN_WORD(wc)
			TKN_OPERATOR(\n)
		eof
	)"
	assert_shell "$input" "$expected"
}

# CMD={bare, bare}  ARGC={2, 2}  REDIR={}  EXPAND={}  QUOTES={}  OP={and_if}
test_op_and()
{
	local input expected
	input="$(cat <<- \eof
		echo a && echo b
		eof
	)"
	expected="$(cat <<- \eof
		TOKENS
			TKN_WORD(echo)
			TKN_WORD(a)
			TKN_OPERATOR(&&)
			TKN_WORD(echo)
			TKN_WORD(b)
			TKN_OPERATOR(\n)
		eof
	)"
	assert_shell "$input" "$expected"
}

# CMD={bare, bare}  ARGC={2, 2}  REDIR={}  EXPAND={}  QUOTES={}  OP={or_if}
test_op_or()
{
	local input expected
	input="$(cat <<- \eof
		echo a || echo b
		eof
	)"
	expected="$(cat <<- \eof
		TOKENS
			TKN_WORD(echo)
			TKN_WORD(a)
			TKN_OPERATOR(||)
			TKN_WORD(echo)
			TKN_WORD(b)
			TKN_OPERATOR(\n)
		eof
	)"
	assert_shell "$input" "$expected"
}

# CMD={bare, bare, bare}  ARGC={1, 1, 1}  REDIR={}  EXPAND={}  QUOTES={}  OP={pipe}
test_op_pipe_chain()
{
	local input expected
	input="$(cat <<- \eof
		ls | wc | cat
		eof
	)"
	expected="$(cat <<- \eof
		TOKENS
			TKN_WORD(ls)
			TKN_OPERATOR(|)
			TKN_WORD(wc)
			TKN_OPERATOR(|)
			TKN_WORD(cat)
			TKN_OPERATOR(\n)
		eof
	)"
	assert_shell "$input" "$expected"
}

# CMD={bare, bare, bare}  ARGC={1, 1, 1}  REDIR={}  EXPAND={}  QUOTES={}  OP={pipe, and_if}
test_op_mixed()
{
	local input expected
	input="$(cat <<- \eof
		ls | wc && env
		eof
	)"
	expected="$(cat <<- \eof
		TOKENS
			TKN_WORD(ls)
			TKN_OPERATOR(|)
			TKN_WORD(wc)
			TKN_OPERATOR(&&)
			TKN_WORD(env)
			TKN_OPERATOR(\n)
		eof
	)"
	assert_shell "$input" "$expected"
}

# Operators delimit a word on their own, so dropping the blanks around them
# must not change the token stream.
# CMD={bare, bare}  ARGC={1, 1}  REDIR={}  EXPAND={}  QUOTES={}  OP={and_if|adjacent}
test_op_and_adjacent()
{
	local input expected
	input="$(cat <<- \eof
		a&&b
		eof
	)"
	expected="$(cat <<- \eof
		TOKENS
			TKN_WORD(a)
			TKN_OPERATOR(&&)
			TKN_WORD(b)
			TKN_OPERATOR(\n)
		eof
	)"
	assert_shell "$input" "$expected"
}

# CMD={bare, bare}  ARGC={1, 1}  REDIR={}  EXPAND={}  QUOTES={}  OP={pipe|adjacent}
test_op_pipe_adjacent()
{
	local input expected
	input="$(cat <<- \eof
		a|b
		eof
	)"
	expected="$(cat <<- \eof
		TOKENS
			TKN_WORD(a)
			TKN_OPERATOR(|)
			TKN_WORD(b)
			TKN_OPERATOR(\n)
		eof
	)"
	assert_shell "$input" "$expected"
}

# ----- Grouping ---------------------------------------------------------------

# CMD={bare}  ARGC={1}  REDIR={}  EXPAND={}  QUOTES={}  OP={}  GROUP={paren|spaced}
test_group_paren()
{
	local input expected
	input="$(cat <<- \eof
		( ls )
		eof
	)"
	expected="$(cat <<- \eof
		TOKENS
			TKN_OPERATOR(()
			TKN_WORD(ls)
			TKN_OPERATOR())
			TKN_OPERATOR(\n)
		eof
	)"
	assert_shell "$input" "$expected"
}

# Parentheses are operators, so they delimit the word next to them without
# needing a blank - `(ls)` must not lex as one word.
# CMD={bare}  ARGC={1}  REDIR={}  EXPAND={}  QUOTES={}  OP={}  GROUP={paren|adjacent}
test_group_paren_adjacent()
{
	local input expected
	input="$(cat <<- \eof
		(ls)
		eof
	)"
	expected="$(cat <<- \eof
		TOKENS
			TKN_OPERATOR(()
			TKN_WORD(ls)
			TKN_OPERATOR())
			TKN_OPERATOR(\n)
		eof
	)"
	assert_shell "$input" "$expected"
}

# CMD={bare, bare, bare}  ARGC={1, 1, 1}  REDIR={}  EXPAND={}  QUOTES={}
# OP={and_if, pipe}  GROUP={paren|spaced}
test_group_paren_pipeline()
{
	local input expected
	input="$(cat <<- \eof
		( ls && wc ) | cat
		eof
	)"
	expected="$(cat <<- \eof
		TOKENS
			TKN_OPERATOR(()
			TKN_WORD(ls)
			TKN_OPERATOR(&&)
			TKN_WORD(wc)
			TKN_OPERATOR())
			TKN_OPERATOR(|)
			TKN_WORD(cat)
			TKN_OPERATOR(\n)
		eof
	)"
	assert_shell "$input" "$expected"
}

# ----- Whitespace -------------------------------------------------------------

# CMD={bare}  ARGC={2}  REDIR={}  EXPAND={}  QUOTES={}  OP={}
test_whitespace_tab()
{
	local input expected
	input="$(cat <<- \eof
		echo	hello
		eof
	)"
	expected="$(cat <<- \eof
		TOKENS
			TKN_WORD(echo)
			TKN_WORD(hello)
			TKN_OPERATOR(\n)
		eof
	)"
	assert_shell "$input" "$expected"
}

# ----- Flags ------------------------------------------------------------------

# CMD={bare}  ARGC={2}  REDIR={}  EXPAND={}  QUOTES={double|bare}  OP={}
test_flag_quotes()
{
	local input expected
	input="$(cat <<- \eof
		echo "hello"
		eof
	)"
	expected="$(cat <<- \eof
		TOKENS
			TKN_WORD(echo)
			TKN_WORD("hello") TKN_HAS_QUOTES
			TKN_OPERATOR(\n)
		eof
	)"
	assert_shell "$input" "$expected"
}

# CMD={bare}  ARGC={2}  REDIR={}  EXPAND={bare}  QUOTES={}  OP={}
test_flag_expansion()
{
	local input expected
	input="$(cat <<- \eof
		echo $HOME
		eof
	)"
	expected="$(cat <<- \eof
		TOKENS
			TKN_WORD(echo)
			TKN_WORD($HOME) TKN_HAS_EXPANSION
			TKN_OPERATOR(\n)
		eof
	)"
	assert_shell "$input" "$expected"
}

# CMD={bare}  ARGC={2}  REDIR={}  EXPAND={enclosed}  QUOTES={double|bare}  OP={}
test_flag_both()
{
	local input expected
	input="$(cat <<- \eof
		echo "$HOME"
		eof
	)"
	expected="$(cat <<- \eof
		TOKENS
			TKN_WORD(echo)
			TKN_WORD("$HOME") TKN_HAS_QUOTES TKN_HAS_EXPANSION
			TKN_OPERATOR(\n)
		eof
	)"
	assert_shell "$input" "$expected"
}

# CMD={bare}  ARGC={2}  REDIR={}  EXPAND={}  QUOTES={double|bare}  OP={}
test_flag_empty_quotes()
{
	local input expected
	input="$(cat <<- \eof
		echo ""
		eof
	)"
	expected="$(cat <<- \eof
		TOKENS
			TKN_WORD(echo)
			TKN_WORD("") TKN_HAS_QUOTES
			TKN_OPERATOR(\n)
		eof
	)"
	assert_shell "$input" "$expected"
}

# CMD={bare}  ARGC={2}  REDIR={}  EXPAND={question|bare}  QUOTES={}  OP={}
test_flag_special_param()
{
	local input expected
	input="$(cat <<- \eof
		echo $?
		eof
	)"
	expected="$(cat <<- \eof
		TOKENS
			TKN_WORD(echo)
			TKN_WORD($?) TKN_HAS_EXPANSION
			TKN_OPERATOR(\n)
		eof
	)"
	assert_shell "$input" "$expected"
}

# CMD={bare}  ARGC={2}  REDIR={}  EXPAND={}  QUOTES={double|prefix}  OP={}
test_flag_quote_concat()
{
	local input expected
	input="$(cat <<- \eof
		echo "foo"bar
		eof
	)"
	expected="$(cat <<- \eof
		TOKENS
			TKN_WORD(echo)
			TKN_WORD("foo"bar) TKN_HAS_QUOTES
			TKN_OPERATOR(\n)
		eof
	)"
	assert_shell "$input" "$expected"
}

# CMD={bare}  ARGC={2}  REDIR={}  EXPAND={suffix}  QUOTES={}  OP={}
test_flag_mid_word_expansion()
{
	local input expected
	input="$(cat <<- \eof
		echo love$HOME
		eof
	)"
	expected="$(cat <<- \eof
		TOKENS
			TKN_WORD(echo)
			TKN_WORD(love$HOME) TKN_HAS_EXPANSION
			TKN_OPERATOR(\n)
		eof
	)"
	assert_shell "$input" "$expected"
}

# CMD={bare}  ARGC={2}  REDIR={}  EXPAND={}  QUOTES={single|bare}  OP={}
test_flag_single_quotes()
{
	local input expected
	input="$(cat <<- \eof
		echo 'hello'
		eof
	)"
	expected="$(cat <<- \eof
		TOKENS
			TKN_WORD(echo)
			TKN_WORD('hello') TKN_HAS_QUOTES
			TKN_OPERATOR(\n)
		eof
	)"
	assert_shell "$input" "$expected"
}

# ----- Miscellaneous cases ----------------------------------------------------

# CMD={}  ARGC={0}  REDIR={}  EXPAND={}  QUOTES={}  OP={}
test_empty_input()
{
	local input expected
	input="$(cat <<- \eof

		eof
	)"
	expected="$(cat <<- \eof
		TOKENS
			TKN_OPERATOR(\n)
		eof
	)"
	assert_shell "$input" "$expected"
}

# CMD={}  ARGC={0}  REDIR={}  EXPAND={}  QUOTES={}  OP={}
test_only_spaces()
{
	local input expected
	input="$(cat <<- \eof
		  	 	
		eof
	)"
	expected="$(cat <<- \eof
		TOKENS
			TKN_OPERATOR(\n)
		eof
	)"
	assert_shell "$input" "$expected"
}

# CMD={bare}  ARGC={2}  REDIR={}  EXPAND={}  QUOTES={double|unclosed}  OP={}
test_unclosed_double_quote()
{
	local input expected
	input="$(cat <<- \eof
		echo "foo
		eof
	)"
	expected="$(cat <<- \eof
		TOKENS
			TKN_WORD(echo)
			TKN_WORD("foo)
			TKN_OPERATOR(\n)
		eof
	)"
	assert_shell "$input" "$expected"
}

# CMD={bare}  ARGC={2}  REDIR={}  EXPAND={}  QUOTES={single|unclosed}  OP={}
test_unclosed_single_quote()
{
	local input expected
	input="$(cat <<- \eof
		echo 'foo
		eof
	)"
	expected="$(cat <<- \eof
		TOKENS
			TKN_WORD(echo)
			TKN_WORD('foo)
			TKN_OPERATOR(\n)
		eof
	)"
	assert_shell "$input" "$expected"
}

# CMD={}  ARGC={}  REDIR={}  EXPAND={}  QUOTES={}  OP={}  OTHER={comment|bare}
test_comment_only()
{
	local input expected
	input="$(cat <<- \eof
		# comment
		eof
	)"
	expected="$(cat <<- \eof
		TOKENS
			TKN_OPERATOR(\n)
		eof
	)"
	assert_shell "$input" "$expected"
}

# CMD={bare}  ARGC={2}  REDIR={}  EXPAND={}  QUOTES={}  OP={}  OTHER={comment|suffix}
test_comment_after_cmd()
{
	local input expected
	input="$(cat <<- \eof
		echo hi # comment
		eof
	)"
	expected="$(cat <<- \eof
		TOKENS
			TKN_WORD(echo)
			TKN_WORD(hi)
			TKN_OPERATOR(\n)
		eof
	)"
	assert_shell "$input" "$expected"
}
