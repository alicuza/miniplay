#!/bin/bash

_run_functions()
{
	local func passed=0 failed=0 result=0

	for func in $(awk '/^test_[a-zA-Z0-9_]*\(\)/ {print $1}' "$1"); do
		func="${func%()}"
		printf "  %-50s " "$func"
		if "$func"; then
			((passed++))
		else
			((failed++))
			result=1
		fi
	done
	printf "%d passed, %d failed\n" "$passed" "$failed"
	return $result
}

run()
{
	local name result=0

	for test_file in test/test_*; do
		name="${test_file##*/}"
		echo "Running: ${name%.sh}"
		case $test_file in
			*builtin*|*search_execution*)
				./"$test_file" || result=1 ;;
			*)
				. ./"$test_file"
				_run_functions "$test_file" || result=1
				;;
		esac
	done
	return $result
}

eval run
