. test/helpers.sh

# ------------------------------------------------------------------------------
# Parser tree tests
# ------------------------------------------------------------------------------
#
# test_parse.sh pins the token stream; these pin what the reductions build out
# of it - the node tree the executor actually walks. A token-level test cannot
# catch a handler that attaches a chain to the wrong node, which is the class
# of bug this file exists for.
#
# Node shapes printed by --scope=command:
#
#   NODE_PIPELINE [next N] [command_head N]
#       next          following pipeline in the list (`;`-style, `&&`, `||`)
#       command_head  first command of this pipeline
#       FLAG_AND_IF / FLAG_OR_IF mark how it joins the pipeline before it
#
#   NODE_COMMAND [next N] [arg_head N] [redir_head N]
#       next          following command in the same pipeline (`|`)
#       FLAG_SUBSHELL makes arg_head the inner pipeline chain instead
#
#   NODE_ARG(word) [next N]        argument chain, first entry is the command
#   NODE_REDIR(op target) [next N] redirection chain; a heredoc prints its body
#
# Dimensions used in per-test annotations:
#
#   SHAPE   what the tree is being pinned for   command | pipeline | list
#                                               | redir | heredoc | subshell
#
#   DEPTH   nesting of the construct            flat | nested
#
#   COUNT   repetitions of the construct        1 | N
#
# ------------------------------------------------------------------------------

# ----- Simple commands --------------------------------------------------------

# SHAPE={command}  DEPTH={flat}  COUNT={1}
test_tree_simple()
{
	local input expected
	input="$(cat <<- \eof
		ls
		eof
	)"
	expected="$(cat <<- \eof
		COMMAND
			[id 1] NODE_ARG(ls) [next 0]
			[id 2] NODE_COMMAND [next 0] [arg_head 1] [redir_head 0]
			[id 3] NODE_PIPELINE [next 0] [command_head 2]
		STATUS
			0
		eof
	)"
	assert_shell "$input" "$expected"
}

# The command name is simply the head of the argument chain, not a field of
# its own - argv[0] falls out of the same walk that collects the operands.
# SHAPE={command}  DEPTH={flat}  COUNT={N}
test_tree_args()
{
	local input expected
	input="$(cat <<- \eof
		echo hi there
		eof
	)"
	expected="$(cat <<- \eof
		COMMAND
			[id 1] NODE_ARG(echo) [next 3]
			[id 2] NODE_COMMAND [next 0] [arg_head 1] [redir_head 0]
			[id 3] NODE_ARG(hi) [next 4]
			[id 4] NODE_ARG(there) [next 0]
			[id 5] NODE_PIPELINE [next 0] [command_head 2]
		eof
	)"
	assert_shell "$input" "$expected"
}

# ----- Pipelines --------------------------------------------------------------

# A pipeline is one NODE_PIPELINE holding a chain of commands linked by
# `next`, so the fork loop has a single list to walk.
# SHAPE={pipeline}  DEPTH={flat}  COUNT={1}
test_tree_pipe()
{
	local input expected
	input="$(cat <<- \eof
		ls | wc
		eof
	)"
	expected="$(cat <<- \eof
		COMMAND
			[id 1] NODE_ARG(ls) [next 0]
			[id 2] NODE_COMMAND [next 5] [arg_head 1] [redir_head 0]
			[id 3] NODE_PIPELINE [next 0] [command_head 2]
			[id 4] NODE_ARG(wc) [next 0]
			[id 5] NODE_COMMAND [next 0] [arg_head 4] [redir_head 0]
		eof
	)"
	assert_shell "$input" "$expected"
}

# SHAPE={pipeline}  DEPTH={flat}  COUNT={N}
test_tree_pipe_chain()
{
	local input expected
	input="$(cat <<- \eof
		ls | wc | cat
		eof
	)"
	expected="$(cat <<- \eof
		COMMAND
			[id 1] NODE_ARG(ls) [next 0]
			[id 2] NODE_COMMAND [next 5] [arg_head 1] [redir_head 0]
			[id 3] NODE_PIPELINE [next 0] [command_head 2]
			[id 4] NODE_ARG(wc) [next 0]
			[id 5] NODE_COMMAND [next 7] [arg_head 4] [redir_head 0]
			[id 6] NODE_ARG(cat) [next 0]
			[id 7] NODE_COMMAND [next 0] [arg_head 6] [redir_head 0]
		eof
	)"
	assert_shell "$input" "$expected"
}

# ----- Lists ------------------------------------------------------------------

# The operator is recorded on the right-hand pipeline, which is what lets the
# executor decide whether to run it from the status of the one before.
# SHAPE={list}  DEPTH={flat}  COUNT={1}
test_tree_and_if()
{
	local input expected
	input="$(cat <<- \eof
		echo a && echo b
		eof
	)"
	expected="$(cat <<- \eof
		COMMAND
			[id 1] NODE_ARG(echo) [next 3]
			[id 2] NODE_COMMAND [next 0] [arg_head 1] [redir_head 0]
			[id 3] NODE_ARG(a) [next 0]
			[id 4] NODE_PIPELINE [next 8] [command_head 2]
			[id 5] NODE_ARG(echo) [next 7]
			[id 6] NODE_COMMAND [next 0] [arg_head 5] [redir_head 0]
			[id 7] NODE_ARG(b) [next 0]
			[id 8] NODE_PIPELINE [next 0] [command_head 6] FLAG_AND_IF
		eof
	)"
	assert_shell "$input" "$expected"
}

# SHAPE={list}  DEPTH={flat}  COUNT={1}
test_tree_or_if()
{
	local input expected
	input="$(cat <<- \eof
		echo a || echo b
		eof
	)"
	expected="$(cat <<- \eof
		COMMAND
			[id 1] NODE_ARG(echo) [next 3]
			[id 2] NODE_COMMAND [next 0] [arg_head 1] [redir_head 0]
			[id 3] NODE_ARG(a) [next 0]
			[id 4] NODE_PIPELINE [next 8] [command_head 2]
			[id 5] NODE_ARG(echo) [next 7]
			[id 6] NODE_COMMAND [next 0] [arg_head 5] [redir_head 0]
			[id 7] NODE_ARG(b) [next 0]
			[id 8] NODE_PIPELINE [next 0] [command_head 6] FLAG_OR_IF
		eof
	)"
	assert_shell "$input" "$expected"
}

# ----- Redirections -----------------------------------------------------------

# SHAPE={redir}  DEPTH={flat}  COUNT={1}
test_tree_redir_in()
{
	local input expected
	input="$(cat <<- \eof
		cat < in
		eof
	)"
	expected="$(cat <<- \eof
		COMMAND
			[id 1] NODE_ARG(cat) [next 0]
			[id 2] NODE_REDIR(< in) [next 0] REDIR_IN
			[id 3] NODE_COMMAND [next 0] [arg_head 1] [redir_head 2]
			[id 4] NODE_PIPELINE [next 0] [command_head 3]
		eof
	)"
	assert_shell "$input" "$expected"
}

# SHAPE={redir}  DEPTH={flat}  COUNT={1}
test_tree_redir_out()
{
	local input expected
	input="$(cat <<- \eof
		echo hi > out
		eof
	)"
	expected="$(cat <<- \eof
		COMMAND
			[id 1] NODE_ARG(echo) [next 3]
			[id 2] NODE_COMMAND [next 0] [arg_head 1] [redir_head 4]
			[id 3] NODE_ARG(hi) [next 0]
			[id 4] NODE_REDIR(> out) [next 0] REDIR_OUT
			[id 5] NODE_PIPELINE [next 0] [command_head 2]
		eof
	)"
	assert_shell "$input" "$expected"
}

# SHAPE={redir}  DEPTH={flat}  COUNT={1}
test_tree_redir_append()
{
	local input expected
	input="$(cat <<- \eof
		echo hi >> out
		eof
	)"
	expected="$(cat <<- \eof
		COMMAND
			[id 1] NODE_ARG(echo) [next 3]
			[id 2] NODE_COMMAND [next 0] [arg_head 1] [redir_head 4]
			[id 3] NODE_ARG(hi) [next 0]
			[id 4] NODE_REDIR(>> out) [next 0] REDIR_APPEND
			[id 5] NODE_PIPELINE [next 0] [command_head 2]
		eof
	)"
	assert_shell "$input" "$expected"
}

# Redirections keep source order in one chain, which is what makes a later
# one on the same stream win at exec time.
# SHAPE={redir}  DEPTH={flat}  COUNT={N}
test_tree_redir_multiple()
{
	local input expected
	input="$(cat <<- \eof
		cat < in > out
		eof
	)"
	expected="$(cat <<- \eof
		COMMAND
			[id 1] NODE_ARG(cat) [next 0]
			[id 2] NODE_REDIR(< in) [next 4] REDIR_IN
			[id 3] NODE_COMMAND [next 0] [arg_head 1] [redir_head 2]
			[id 4] NODE_REDIR(> out) [next 0] REDIR_OUT
			[id 5] NODE_PIPELINE [next 0] [command_head 3]
		eof
	)"
	assert_shell "$input" "$expected"
}

# A redirection written before the command name belongs to the same command:
# it lands on the redirection chain and leaves the argument chain alone.
# SHAPE={redir}  DEPTH={flat}  COUNT={1}
test_tree_redir_prefix()
{
	local input expected
	input="$(cat <<- \eof
		> out echo hi
		eof
	)"
	expected="$(cat <<- \eof
		COMMAND
			[id 1] NODE_REDIR(> out) [next 0] REDIR_OUT
			[id 2] NODE_ARG(echo) [next 4]
			[id 3] NODE_COMMAND [next 0] [arg_head 2] [redir_head 1]
			[id 4] NODE_ARG(hi) [next 0]
			[id 5] NODE_PIPELINE [next 0] [command_head 3]
		eof
	)"
	assert_shell "$input" "$expected"
}

# ----- Heredocs ---------------------------------------------------------------

# The node prints the collected body, not the delimiter: the io_here reduction
# leaves a placeholder and the body is attached once it has been read, which
# is the only way the executor can reach it.
# SHAPE={heredoc}  DEPTH={flat}  COUNT={1}
test_tree_heredoc()
{
	local input expected
	input="$(cat <<- \eof
		cat << EOF
		hello
		EOF
		eof
	)"
	expected="$(cat <<- \eof
		COMMAND
			[id 1] NODE_ARG(cat) [next 0]
			[id 2] NODE_REDIR(<< EOF) [next 0] REDIR_HERE
			[id 3] NODE_COMMAND [next 0] [arg_head 1] [redir_head 2]
			[id 4] NODE_PIPELINE [next 0] [command_head 3]
		eof
	)"
	assert_shell "$input" "$expected"
}

# A quoted delimiter is recorded on the node, since it is what decides whether
# the body expands when it is fed to the command.
# SHAPE={heredoc}  DEPTH={flat}  COUNT={1}
test_tree_heredoc_quoted()
{
	local input expected
	input="$(cat <<- \eof
		cat << 'EOF'
		hello
		EOF
		eof
	)"
	expected="$(cat <<- \eof
		COMMAND
			[id 1] NODE_ARG(cat) [next 0]
			[id 2] NODE_REDIR(<< EOF) [next 0] REDIR_HERE REDIR_HAS_QUOTES
			[id 3] NODE_COMMAND [next 0] [arg_head 1] [redir_head 2]
			[id 4] NODE_PIPELINE [next 0] [command_head 3]
		eof
	)"
	assert_shell "$input" "$expected"
}

# Both bodies are read, in the order the delimiters appeared - the second
# divert cycle is armed while the first one's tokens are replayed.
# SHAPE={heredoc}  DEPTH={flat}  COUNT={N}
test_tree_heredoc_multiple()
{
	local input expected
	input="$(cat <<- \eof
		cat <<A <<B
		one
		A
		two
		B
		eof
	)"
	expected="$(cat <<- \eof
		COMMAND
			[id 1] NODE_ARG(cat) [next 0]
			[id 2] NODE_REDIR(<< A) [next 4] REDIR_HERE
			[id 3] NODE_COMMAND [next 0] [arg_head 1] [redir_head 2]
			[id 4] NODE_REDIR(<< B) [next 0] REDIR_HERE
			[id 5] NODE_PIPELINE [next 0] [command_head 3]
		eof
	)"
	assert_shell "$input" "$expected"
}

# SHAPE={heredoc, pipeline}  DEPTH={flat}  COUNT={1}
test_tree_heredoc_pipe()
{
	local input expected
	input="$(cat <<- \eof
		cat << E | wc
		body
		E
		eof
	)"
	expected="$(cat <<- \eof
		COMMAND
			[id 1] NODE_ARG(cat) [next 0]
			[id 2] NODE_REDIR(<< E) [next 0] REDIR_HERE
			[id 3] NODE_COMMAND [next 6] [arg_head 1] [redir_head 2]
			[id 4] NODE_PIPELINE [next 0] [command_head 3]
			[id 5] NODE_ARG(wc) [next 0]
			[id 6] NODE_COMMAND [next 0] [arg_head 5] [redir_head 0]
		eof
	)"
	assert_shell "$input" "$expected"
}

# An empty delimiter (<< "") quote-removes to "", which matches the first blank
# line -- the heredoc ends there and the rest is parsed as a separate command.
# SHAPE={heredoc}  DEPTH={flat}  COUNT={1}
test_tree_heredoc_empty_delim()
{
	local input expected
	input="$(cat <<- \eof
		cat <<""
		hello

		world
		eof
	)"
	expected="$(cat <<- \eof
		COMMAND
			[id 1] NODE_ARG(cat) [next 0]
			[id 2] NODE_REDIR(<< ) [next 0] REDIR_HERE REDIR_HAS_QUOTES
			[id 3] NODE_COMMAND [next 0] [arg_head 1] [redir_head 2]
			[id 4] NODE_PIPELINE [next 7] [command_head 3]
			[id 5] NODE_ARG(world) [next 0]
			[id 6] NODE_COMMAND [next 0] [arg_head 5] [redir_head 0]
			[id 7] NODE_PIPELINE [next 0] [command_head 6]
		eof
	)"
	assert_shell "$input" "$expected"
}

# An unterminated heredoc warns at EOF and must NOT crash (regression: a NULL
# read_line reached lex_token because the EOF branch never set LEX_AT_EOI).
# SHAPE={heredoc}  DEPTH={flat}  COUNT={1}
test_heredoc_unterminated_eof()
{
	local input expected
	input="$(cat <<- \eof
		cat << END
		foo
		eof
	)"
	expected="$(cat <<- \eof
		ERROR
			shni: warning: here-document delimited by end-of-file (wanted `END')
		STATUS
			0
		eof
	)"
	assert_shell "$input" "$expected"
}

# ----- Subshells --------------------------------------------------------------

# A subshell is a command whose arg_head is the inner pipeline chain rather
# than an argument list, so it slots into a pipeline like any other command.
# SHAPE={subshell}  DEPTH={nested}  COUNT={1}
test_tree_subshell()
{
	local input expected
	input="$(cat <<- \eof
		( ls )
		eof
	)"
	expected="$(cat <<- \eof
		COMMAND
			[id 1] NODE_ARG(ls) [next 0]
			[id 2] NODE_COMMAND [next 0] [arg_head 1] [redir_head 0]
			[id 3] NODE_PIPELINE [next 0] [command_head 2]
			[id 4] NODE_COMMAND [next 0] [arg_head 3] [redir_head 0] FLAG_SUBSHELL
			[id 5] NODE_PIPELINE [next 0] [command_head 4]
		eof
	)"
	assert_shell "$input" "$expected"
}

# SHAPE={subshell, pipeline}  DEPTH={nested}  COUNT={1}
test_tree_subshell_pipeline()
{
	local input expected
	input="$(cat <<- \eof
		( ls | wc )
		eof
	)"
	expected="$(cat <<- \eof
		COMMAND
			[id 1] NODE_ARG(ls) [next 0]
			[id 2] NODE_COMMAND [next 5] [arg_head 1] [redir_head 0]
			[id 3] NODE_PIPELINE [next 0] [command_head 2]
			[id 4] NODE_ARG(wc) [next 0]
			[id 5] NODE_COMMAND [next 0] [arg_head 4] [redir_head 0]
			[id 6] NODE_COMMAND [next 0] [arg_head 3] [redir_head 0] FLAG_SUBSHELL
			[id 7] NODE_PIPELINE [next 0] [command_head 6]
		eof
	)"
	assert_shell "$input" "$expected"
}

# The grouping the bonus needs: the inner `&&` stays inside the subshell and
# the outer `||` joins the group as a whole, rather than binding to `echo b`.
# SHAPE={subshell, list}  DEPTH={nested}  COUNT={N}
test_tree_subshell_nested()
{
	local input expected
	input="$(cat <<- \eof
		( echo a && echo b ) || echo c
		eof
	)"
	expected="$(cat <<- \eof
		COMMAND
			[id 1] NODE_ARG(echo) [next 3]
			[id 2] NODE_COMMAND [next 0] [arg_head 1] [redir_head 0]
			[id 3] NODE_ARG(a) [next 0]
			[id 4] NODE_PIPELINE [next 8] [command_head 2]
			[id 5] NODE_ARG(echo) [next 7]
			[id 6] NODE_COMMAND [next 0] [arg_head 5] [redir_head 0]
			[id 7] NODE_ARG(b) [next 0]
			[id 8] NODE_PIPELINE [next 0] [command_head 6] FLAG_AND_IF
			[id 9] NODE_COMMAND [next 0] [arg_head 4] [redir_head 0] FLAG_SUBSHELL
			[id 10] NODE_PIPELINE [next 14] [command_head 9]
			[id 11] NODE_ARG(echo) [next 13]
			[id 12] NODE_COMMAND [next 0] [arg_head 11] [redir_head 0]
			[id 13] NODE_ARG(c) [next 0]
			[id 14] NODE_PIPELINE [next 0] [command_head 12] FLAG_OR_IF
		eof
	)"
	assert_shell "$input" "$expected"
}

# ----- Syntax errors ----------------------------------------------------------
#
# Rejection is as much a parser behaviour as acceptance, and it is observable
# in two places that have drifted apart before: the message naming the token
# that stopped the parse, and the exit status bash sets to 2.

# An operator with nothing after it is only known to be an error once input
# runs out, so the token blamed is the end of file rather than the operator.
# SHAPE={list}  DEPTH={flat}  COUNT={1}
test_error_trailing_pipe()
{
	local input expected
	input="$(cat <<- \eof
		ls |
		eof
	)"
	expected="$(cat <<- \eof
		ERROR
			shni: syntax error near unexpected token 'end of file'
		STATUS
			2
		eof
	)"
	assert_shell "$input" "$expected"
}

# SHAPE={list}  DEPTH={flat}  COUNT={1}
test_error_trailing_and()
{
	local input expected
	input="$(cat <<- \eof
		echo a &&
		eof
	)"
	expected="$(cat <<- \eof
		ERROR
			shni: syntax error near unexpected token 'end of file'
		STATUS
			2
		eof
	)"
	assert_shell "$input" "$expected"
}

# SHAPE={list}  DEPTH={flat}  COUNT={1}
test_error_leading_pipe()
{
	local input expected
	input="$(cat <<- \eof
		| ls
		eof
	)"
	expected="$(cat <<- \eof
		ERROR
			shni: syntax error near unexpected token '|'
		STATUS
			2
		eof
	)"
	assert_shell "$input" "$expected"
}

# SHAPE={list}  DEPTH={flat}  COUNT={1}
test_error_bare_operator()
{
	local input expected
	input="$(cat <<- \eof
		&&
		eof
	)"
	expected="$(cat <<- \eof
		ERROR
			shni: syntax error near unexpected token '&&'
		STATUS
			2
		eof
	)"
	assert_shell "$input" "$expected"
}

# SHAPE={subshell}  DEPTH={nested}  COUNT={1}
test_error_unclosed_paren()
{
	local input expected
	input="$(cat <<- \eof
		( ls
		eof
	)"
	expected="$(cat <<- \eof
		ERROR
			shni: syntax error near unexpected token 'end of file'
		STATUS
			2
		eof
	)"
	assert_shell "$input" "$expected"
}

# SHAPE={subshell}  DEPTH={flat}  COUNT={1}
test_error_stray_close_paren()
{
	local input expected
	input="$(cat <<- \eof
		ls )
		eof
	)"
	expected="$(cat <<- \eof
		ERROR
			shni: syntax error near unexpected token ')'
		STATUS
			2
		eof
	)"
	assert_shell "$input" "$expected"
}
