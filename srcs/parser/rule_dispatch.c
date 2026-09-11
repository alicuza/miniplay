#include "minishell.h"

/* -------- reduction dispatch (bison rule number) -------------------------- */
static t_rule	rule_dispatch_first(int32_t action)
{
	static const t_rule	rule[16] = {
		{NULL, 0, SYM_EOF},
		{NULL, 2, SYM_ACCEPT},
		{reduce_program, 3, SYM_PROGRAM},
		{NULL, 1, SYM_PROGRAM},
		{reduce_list_append, 3, SYM_COMPLETE_COMMANDS},
		{NULL, 1, SYM_COMPLETE_COMMANDS},
		{NULL, 1, SYM_AND_OR},
		{reduce_and_or_conditional, 4, SYM_AND_OR},
		{reduce_and_or_conditional, 4, SYM_AND_OR},
		{reduce_pipeline_create, 1, SYM_PIPELINE},
		{reduce_pipeline_append, 4, SYM_PIPELINE},
		{NULL, 1, SYM_COMMAND},
		{NULL, 1, SYM_COMMAND},
		{reduce_subshell_redirects, 2, SYM_COMMAND},
		{reduce_subshell, 3, SYM_SUBSHELL},
		{reduce_compound_list, 2, SYM_COMPOUND_LIST}
	};
	return (rule[action]);
}

static t_rule	rule_dispatch_second(int32_t action)
{
	static const t_rule	rule[16] = {
		{reduce_compound_list, 3, SYM_COMPOUND_LIST},
		{reduce_list_append, 3, SYM_TERM},
		{NULL, 1, SYM_TERM},
		{reduce_simple_command_from_suffix, 3, SYM_SIMPLE_COMMAND},
		{reduce_simple_command, 2, SYM_SIMPLE_COMMAND},
		{reduce_simple_command, 1, SYM_SIMPLE_COMMAND},
		{reduce_simple_command_from_suffix, 2, SYM_SIMPLE_COMMAND},
		{reduce_simple_command, 1, SYM_SIMPLE_COMMAND},
		{reduce_from_term, 1, SYM_CMD_NAME},
		{reduce_from_term, 1, SYM_CMD_WORD},
		{NULL, 1, SYM_CMD_PREFIX},
		{reduce_redir_append, 2, SYM_CMD_PREFIX},
		{reduce_cmd_suffix, 1, SYM_CMD_SUFFIX},
		{reduce_cmd_suffix, 2, SYM_CMD_SUFFIX},
		{reduce_cmd_suffix, 1, SYM_CMD_SUFFIX},
		{reduce_cmd_suffix, 2, SYM_CMD_SUFFIX}
	};
	return (rule[action - 16]);
}

static t_rule	rule_dispatch_third(int32_t action)
{
	static const t_rule	rule[14] = {
		{NULL, 1, SYM_REDIRECT_LIST},
		{reduce_redir_append, 2, SYM_REDIRECT_LIST},
		{NULL, 1, SYM_IO_REDIRECT},
		{NULL, 1, SYM_IO_REDIRECT},
		{reduce_io_file, 2, SYM_IO_FILE},
		{reduce_io_file, 2, SYM_IO_FILE},
		{reduce_io_file, 2, SYM_IO_FILE},
		{reduce_from_term, 1, SYM_FILENAME},
		{reduce_io_here, 2, SYM_IO_HERE},
		{NULL, 1, SYM_HERE_END},
		{NULL, 1, SYM_NEWLINE_LIST},
		{NULL, 2, SYM_NEWLINE_LIST},
		{NULL, 1, SYM_LINEBREAK},
		{NULL, 0, SYM_LINEBREAK}
	};
	return (rule[action - 32]);
}

t_rule	get_rule(int32_t action)
{
	if (action < 16)
		return (rule_dispatch_first(action));
	if (action < 32)
		return (rule_dispatch_second(action));
	return (rule_dispatch_third(action));
}
