_assert_run()
{
	local input="$1" scope="$2"
	local block="$(sed 's/^[[:space:]]*//; s/[[:space:]]*$//' <<< "$3")"
	local result delta

	[ -n "$scope" ] || return 0
	result=$(printf "\e[200~%s\e[201~\n" "$input" \
			| ./minishell-debug --no_exec --scope="$scope" 2>/dev/null \
			| sed 's/^[[:space:]]*//; s/[[:space:]]*$//')
	delta="$(diff -u <(printf "%s\n" "$block") <(printf "%s\n" "$result"))" \
		&& echo "OK" || { echo "KO"; echo "$delta"; return 1; }
}

assert_shell()
{
	local input="$1" expected="$2"
	local scope="" block=""
	local result=0

	while IFS= read -r line; do
		case "$line" in
            TOKENS|SYMBOLS|STACK|COMMAND)
                _assert_run "$input" "$scope" "$block" || result=1
                block=""
                case "$line" in
                    TOKENS)
                        scope="tokens" ;;
                    SYMBOLS|STACK)
                        scope="stack" ;;
                    COMMAND)
                        scope="command" ;;
                esac ;;
			*)
				block="${block}${line}"$'\n' ;;
		esac
	done <<< "$expected"
	_assert_run "$input" "$scope" "$block" || result=1
	return $result
}
