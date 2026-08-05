# Drives the shell once for one section of an expected block.
#
# Input is sent wrapped in bracketed paste so a whole multi-line construct
# (a heredoc, a subshell spanning lines) arrives as a single input, and
# --no_exec keeps it a parser test: nothing is run, so a heredoc body never
# reaches a real command and the node arena survives to be printed.
_capture()
{
	local input="$1" scope="$2"

	if [ "$scope" = "status" ]; then
		printf "\e[200~%s\e[201~\n" "$input" \
			| ./minishell-debug --no_exec --states=none --arenas=none \
				>/dev/null 2>&1
		echo "$?"
	elif [ "$scope" = "error" ]; then
		printf "\e[200~%s\e[201~\n" "$input" \
			| ./minishell-debug --no_exec --states=none --arenas=none \
				2>&1 >/dev/null \
			| sed -n '/^minishell: /p'
	else
		printf "\e[200~%s\e[201~\n" "$input" \
			| ./minishell-debug --no_exec --scope="$scope" --states=none \
				--arenas=none 2>/dev/null
	fi
}

_assert_run()
{
	local input="$1" scope="$2"
	local block="$(sed 's/^[[:space:]]*//; s/[[:space:]]*$//' <<< "$3")"
	local result delta

	[ -n "$scope" ] || return 0
	result=$(_capture "$input" "$scope" \
			| sed '/^[[:space:]]*$/d; /^--- .* ---/d; /^(state [0-9]/d' \
			| sed 's/^[[:space:]]*//; s/[[:space:]]*$//')
	delta="$(diff -u <(printf "%s\n" "$block") <(printf "%s\n" "$result"))" \
		|| { echo "KO ($scope)"; echo "$delta"; return 1; }
}

# An expected block is a sequence of sections, each introduced by the name of
# what it describes, so one test can pin several views of the same input:
#
#   TOKENS   the token stream out of the lexer
#   STACK    the parser's symbol stack (SYMBOLS is accepted as an alias)
#   COMMAND  the node tree the reductions built
#   ERROR    diagnostics the shell printed
#   STATUS   the exit status it left behind
assert_shell()
{
	local input="$1" expected="$2"
	local scope="" block=""
	local result=0

	while IFS= read -r line; do
		case "$line" in
			TOKENS|SYMBOLS|STACK|TRACE|COMMAND|ERROR|STATUS)
				_assert_run "$input" "$scope" "$block" || result=1
				block=""
				case "$line" in
					TOKENS)
						scope="tokens" ;;
					SYMBOLS|STACK)
						scope="stack" ;;
					TRACE)
						scope="trace" ;;
					COMMAND)
						scope="command" ;;
					ERROR)
						scope="error" ;;
					STATUS)
						scope="status" ;;
				esac ;;
			*)
				block="${block}${line}"$'\n' ;;
		esac
	done <<< "$expected"
	_assert_run "$input" "$scope" "$block" || result=1
	[ $result -eq 0 ] && echo "OK"
	return $result
}
