%{
/* Grammar specification for minishell LALR analysis.
 * Based on POSIX.1-2024 §2.10.2, trimmed to mandatory feature set.
 */
%}


%token WORD NEWLINE PIPE LESS GREAT DLESS DGREAT AND_IF OR_IF OPAR CPAR

%verbose

%start program

%%

program
	: linebreak complete_commands linebreak
	| linebreak
	;

complete_commands
	: complete_commands separator list
	| list
	;

list
	: pipeline
	| list AND_IF linebreak pipeline
	| list OR_IF  linebreak pipeline
	;

pipeline
	: command
	| pipeline PIPE linebreak command
	;

command
	: simple_command
	| subshell
	| subshell redirect_list
	;

subshell
	: OPAR compound_list CPAR
	;

compound_list
	: linebreak term
	| linebreak term separator
	;

term
	: term separator list
	| list
	;

simple_command
	: cmd_prefix cmd_word cmd_suffix
	| cmd_prefix cmd_word
	| cmd_prefix
	| cmd_name cmd_suffix
	| cmd_name
	;

cmd_name
	: WORD
	;

cmd_word
	: WORD
	;

cmd_prefix
	: io_redirect
	| cmd_prefix io_redirect
	;

cmd_suffix
	: io_redirect
	| cmd_suffix io_redirect
	| WORD
	| cmd_suffix WORD
	;

redirect_list
	: io_redirect
	| redirect_list io_redirect
	;

io_redirect
	: io_file
	| io_here
	;

io_file
	: LESS filename
	| GREAT filename
	| DGREAT filename
	;

filename
	: WORD
	;

io_here
	: DLESS here_end
	;

here_end
	: WORD
	;

separator
	: NEWLINE
	| separator NEWLINE
	;

linebreak
	: separator
	| /* empty */
	;

%%
