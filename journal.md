# Journal

- [Journal](#journal)
  - [minishell](#minishell)
  - [personal](#personal)
- [Structure](#structure)
- [Schedule](#schedule)
- [TODO](#todo)
  - [minishell](#minishell-1)
  - [longterm](#longterm)
- [Documentation](#documentation)
  - [online](#online)
  - [local](#local)

## minishell

**2026.04.26.**
- read about readline
- set up the repo
- drafted minishell.h
- drafted main.c
- drafted README.md
- added documentation on the readline, posix standard, bash manual
- made todo lists for minishell, longterm
- added description

**2026.04.27.**
- read up on how to reopen closed stdin/out/err fds: `open(/dev/tty)`
- researched the allowed functions to a certain degree

**2026.04.28.**
- added some documentation for termcap library
- decided on using github, with feature branches and pull requests
- heard about interactive mode (when a program waits for inputs)

**2026.04.29.**
- read from posix: quoting, tokenizing, expansion, redirection
- tested stuff out, bash seems to closesly follow the posix version on these topics

**2026.04.30.**
- read from posix: interactive mode, command line editing (vi mode)

**2026.05.01. - 2026.05.04**
- read from posix: the IFS variable and it's effect on variable expansion
- read about the difference between env and set, environment and internal shell variables

**2026.05.05**
- read from posix, [2.3 Token Recognition](https://pubs.opengroup.org/onlinepubs/9799919799/utilities/V3_chap02.html#tag_19_03):

```
Once a token is delimited, it is categorized as required by the grammar in 2.10 Shell Grammar.

In situations where the shell parses its input as a program, once a complete_command has been recognized by the grammar (see 2.10 Shell Grammar), the complete_command shall be executed before the next complete_command is tokenized and parsed.
```
  - so once a token gets "delimited", you immediately parse it, and a recognized command gets immediatly executed before tokenization continues.

**2026.05.06**
- fixed UB in `ft_memmove`, comparing pointers that are not necessarily pointing to the same object in memory. Cast to `intptr_t` first.
- first structure sketched: [Visualisation](https://excalidraw.com/#json=8BAFo2sDfsrJqzire7OOM,YjbRdUHRhwRBObV-QLgAXg)
  - tokens will reference slices of the input string, maintaining the original information such as type of quotes and expansion characters.
  - tokens resulting from expansion will be appended to the token arena and relinked through indices to maintain the correct order of operations.

**2026.05.07.**
- worked on the arena implementation:
  - added `arena_grow` function
  - reworked alignment to be a member of the struct: `arena->align` encodes the type of alignment (if the alignment is set to 0, it should be using default dynamic alignment)

**2026.05.12.**
- `env` has to be a built-in, which it isn't in `bash`
- added description of token recognition rules
- added description of shell grammar

**2026.05.13.**
- created a simple parser with flex and bison
  - got overwhelmed reading the generated `.c` files
- wrote makeshift `bnaf` format grammar rules

**2026.05.14. - 2026.05.16.**
- acquired rene's annotated bash manual pdf.
  - nice as an overview of what is actually relevant for minishell accoring to rene
- read from bash manual about execution, [3.7 Executing Commands](https://www.gnu.org/software/bash/manual/bash.html#Executing-Commands):
  - order of expansion for simple commands: [3.7.1 Simple Command Expansion](https://www.gnu.org/software/bash/manual/bash.html#Simple-Command-Expansion)
  - order of `PATH` search & command execution: [3.7.2 Command Search and Execution](https://www.gnu.org/software/bash/manual/bash.html#Command-Search-and-Execution)
  - commands in pipelines are always executed in a child, [3.2.3 Pipelines](https://www.gnu.org/software/bash/manual/bash.html#Pipelines): "Each command in a multi-command pipeline, where pipes are created, is executed in its own subshell, which is a separate process."
  - the shell has it's own execution environment, that consists of inheritances at invocation (`char **envp`) and modifications through builtins, like `cd` and `export`, or assignments `name=word`
  - commands other than builtins are executed in their seaprate execution environment (`child process`) with values inherited from the shell's environment ("variables marked for `export`"), everything copied by `fork()` (open fds, file creation mask) (additionally, but not required by the project: assignments before the command name, which are injected temporarily into the env for this one command)

**2026.05.17.**
- looked through the `makefiles` of people for inspiration (starred on github)
- used `declare` to see all shell variables listed, which nicely shows what the different attributes of shell variables are

**2026.05.19**
- read from the readline manual, [2.1 Basic Behavior](https://tiswww.cwru.edu/php/chet/readline/readline.html#Basic-Behavior)
  - seems pretty straightforward considering the number of functions we are allowed to use. there is another example in the manual that also handles signals, i still have to look into those more. since the handlers mostly just modify the global variable we're allowed to use, this shouldn't be that hard to implement.
  - this can be the basis for the entrypoint:

```
Here is a function which usefully replaces the standard gets() library function, and has the advantage of no static buffer to overflow:

/* A static variable for holding the line. */
static char *line_read = (char *)NULL;

/* Read a string, and return a pointer to it.
   Returns NULL on EOF. */
char *
rl_gets ()
{
  /* If the buffer has already been allocated,
     return the memory to the free pool. */
  if (line_read)
    {
      free (line_read);
      line_read = (char *)NULL;
    }

  /* Get a line from the user. */
  line_read = readline ("");

  /* If the line has any text in it,
     save it on the history. */
  if (line_read && *line_read)
    add_history (line_read);

  return (line_read);
}
```

- added documentation on signals

**2026.05.20. - 2026.05.21.**
- implemented an entrypoint based on the example found in the readline manual
- implemented a prototype for a prompt that could be extended to be dynamic
- added arenas to the naive implementation
  - had to modify arenas to include a member element sized zero initialized memory area at offset 0
- found out that readline strips the `\n` from the end of the `read_line`, which needs to be reinserted, when the readline is copied to its arena buffer
- added section for stuff that is not required

**2026.05.22.**
- massive restructuring of folder structure and makefile
  - created a separate debug function folder to use for debugging, without including it into the binary otherwise
- added a way to quickly test different arena sizes, inspired by the tests i did for gnl
- started on the tokenizer, it seems to create the correct arena entries.
  - needs more looking at, i forgot how annoying it is to work with indices and arenas. i need to improve the api.

**2026.05.23.**
- started work on the tokenizer
- some naming changes, like `env` to `ctx`

**2026.05.25.**
- mvp tokenizer works.
- modified and added to the debug function:
  - `print_arena` now names the type of arena it is
  - `print_token` and `poison_sentinel` (changes the last byte of the sentinel to `0xff`) are the newest additions.
- need to decide how to track whether a variable included liteal quote chars, because those don't need to be removed by the subsequent quote removal stage

**2026.05.26. - 2026.05.30**
- finished the tokenizer with all rules
- some basic testing and refactoring done -> the annoying bugs are waiting in the dark edges of my code for sure.
- added colour to the prompt so we can differentiate it better from the usual shell

**2026.05.31.**
- attempting to add rule 9 (comments) to the tokenizer - not required, so maybe commented out

**2026.06.01.**
- issue with corrupted redisplay of lines from history discovered and fixed
  *see [2.4.6 Redisplay - rl_expand_prompt](https://tiswww.case.edu/php/chet/readline/readline.html#index-rl_005fexpand_005fprompt)*
```
Applications may indicate that the prompt contains characters that take up no physical screen space when displayed by bracketing a sequence of such characters with the special markers RL_PROMPT_START_IGNORE and RL_PROMPT_END_IGNORE (declared in readline.h as ‘\001’ and ‘\002’, respectively). This may be used to embed terminal-specific escape sequences in prompts. If you don’t use these indicators, redisplay will likely produce screen contents that don’t match the line buffer.
```

  - wrapping the escape sequences in `\001`, `\002` fixed it
  - should have noticed that it started happening after I implemented colours
- stated adding `here-doc` processing steps to documentation
  - might need to rethink my flow: added a proposal to [Parsing](#parsing)
- started implementing `here-doc`

**2026.06.02.**
- added return value (default of 0 for now) and hostname to the prompt
- made the prompt more robust. can handle any size of hostname or cwd, that the system permits
- added `arena_itoa.c` to `libft/arena/`
- renamed `token->type` to `token->token_type`, in preparation for potentially implementing `token->delim_type`

**2026.06.03. - 2026.06.08.**
- worked on understanding the intricacies of the LALR(1) parser architecture, and how to integrate it with my arenas: created some notes on the grammar and manual table generation algorithm.
- decided to restructure the architecture more thoroughly. The token structs will be replaced by symbol node struct that can live in the stack arena (replacing the token arena)
- prompt reworked: colored, dynamic with `user@hostname:cwd[exitcode]`
  - uses `AT_PROMPT` arena with `arena_clear()` per iteration
  - escape sequences wrapped in `\001`/`\002` to prevent readline redisplay corruption

**2026.06.08. - 2026.06.12.**
- refactored massively:
  - the readline input is not the initial source of input, until a token is delimited
  - on delimiting, token bodies are copied into the string arena
  - on shifting, tokens are created on the stack.
  - added multiple helper functions (`is_name_start`, `is_name_body`) to `string_utils.c`
  - created new debug functions
- added `?` (`SPECIAL_PARAM_SET`) to be recognized in the lexer
- fixed `arena_alloc` OOM corruption with `arena_grow_error_handler` because checking for the return of the sentinel node is used for other cases (in the lexer for instance)
- barebones parser: `get_lookahead()` → tokenize one token → `shift_symbol()` → repeat
  - `t_rule` struct and `t_reduce` typedef defined in types.h
- following: adding reduce functions & sparse goto/action tables, expansion (+ field splitting & quote removal)

**2026.06.13.**
- replaced the token system: `t_token` is now only used for lookahead
  - `t_symbol` is used for grammar symbols
  - on delimiting, token body is copied into `AT_STRING` arena
- debug system refactored:
  - `--scope=<flags>` to filter debug output (SCOPE_TOKEN, SCOPE_SYMBOLS, SCOPE_STACK, SCOPE_COMMAND)
  - `--no_exec` flag to skip execution when testing lexer/parser
  - separate debug source files, only compiled with `-DDEBUG`
- implemented non-interactive mode:
  - `is_interactive` checked via `isatty(STDIN_FILENO)`
  - `fstat(STDIN_FILENO)` check to catch closed stdin
  - non-interactive: prompt=NULL, `rl_outstream=stderr`
  - continuation prompt: `"> "` hardcoded

**2026.06.26. - 2026.06.28.**
- discovered a problem in how my lexer sets flags. I can either:
  - refactor the tokenizer to use flags on encountering certain chars, like expansions and quotes.
  - just hack it together for now as a special case in `try_as_quote` (will be renamed to `try_as_matched_pair`)
  - i might still simplify the redirection operators as well (rule 2, 3, 6 -> one rediuirection rule?)
- built out the test infrastructure:
  - `test/runner.sh`: discovers and runs `test_*.sh` files
  - `test/base_test.sh`: `compare_bash_shni` to diff minishell vs bash output
  - `test/helpers.sh`: `assert_shell` with scope and no-exec markers (`--scope`/`--no_exec`)
  - `test/test_parse.sh`: parser tokenizer tests - very happy with how this turned out
  - `make test` target wired: builds release and debug binaries, runs runner.sh - libft 
    - should refactor libft to store noth versions of the build, release and debug, so it doesn't need to recompile libft, if nothing changed there.
- refactored debug output to use `FILE *out` when this output is relevant to the test being performed
- added `parse_debug_args()` to handle `--scope` and `--no_exec` flags

**2026.07.05.**
- discussed non-interactive input design:
  - decided to drop readline for non-interactive mode entirely
  - instead: `read()` all of stdin into `AT_INPUT` arena in a loop
  - no `rl_outstream` juggling, no echo suppression needed
  - clarified that the grammar is indifferent to `\n` reaching the lexer:
    `linebreak : newline_list | /* empty */` means both paths work

**2026.07.06.**
- fixed prompt.c off-by-one: `arena_alloc(prompt, size, 1) - 1`
- cleaned up test_parse.sh annotation dimension grid and expected indentation
- non-interactive mode: `readline(NULL)` with `rl_outstream=stderr`, no history
- change how reductions onto AT_STACK work, only logical stack operations
- AT_INPUT is optional - bulk-read alternative for non-interactive; preferred is get_next_line line-by-line
- the complete AST is built in AT_CMD

**2026.07.08.**
- settled the AST node architecture:
  - node types: NODE_PIPELINE, NODE_COMMAND, NODE_ARG, NODE_REDIR
  - NODE_SUBSHELL - NODE_COMMAND with FLAG_SUBSHELL, data.command.args = inner pipeline
  - root is the first NODE_PIPELINE; sequential pipelines chain via `next`
  - `|` extends the pipeline chain: NODE_COMMAND chained via `next` within a NODE_PIPELINE
  - NODE_PIPELINE: data.pipeline.command → first NODE_COMMAND
  - NODE_COMMAND: data.command.args → NODE_ARG chain, data.command.redirs → NODE_REDIR chain

#### personal

**2026.04.30**

```sh
# less colors for man pages
export LESS=-R

export LESS_TERMCAP_mb=$'\e[01;38;5;211m' # begin blinking
export LESS_TERMCAP_md=$'\e[01;38;5;116m' # begin bold
export LESS_TERMCAP_me=$'\e[0m'           # end mode

export LESS_TERMCAP_so=$'\e[30;38;5;222m' # begin standout-mode - info box
export LESS_TERMCAP_se=$'\e[0m'           # end standout-mode

export LESS_TERMCAP_us=$'\e[04;38;5;183m' # begin underline
export LESS_TERMCAP_ue=$'\e[0m'           # end underline
```

**documentation**
- `man console_codes`
- [ANSI escape code](https://en.wikipedia.org/wiki/ANSI_escape_code)

**2026.05.17.**
- read about [`trap`](https://www.gnu.org/software/bash/manual/bash.html#index-trap): executes a given command when a given signal is intercepted.
- learned neat bash trick: `set -e; : ${parameter:?word}` to close the shell if the parameter doesn't exist in the execution environment.
- read about [`job control`](https://www.gnu.org/software/bash/manual/bash.html#Job-Control)

**2026.05.23.**
- structs are automatically padded to align with the biggest member type.
  - for my arenas it is then unnecessary to even align them, since they are automatically aligned by the way sturcts are padded

**2026.07.03.**
- read about `GLR` (Generalised LR) parsers in the [bison manual](TODO: add link)
  - parsers that branch on unresolved `shift`/`reduce` and `reduce`/`reduce` conflicts
    and join on reaching the same symbol or vanishing on encountering a parse error
- archived all scratch notes under `literature/minishell_notes/`

### Schedule

- starting on 2026.05.07. Nikita is on vacation until the 2026.05.22.
- 1.4. - 7.4.:
  - finish tokenization for here-doc
  - work on implementing the shell grammar

### TODO

#### minishell

**research & documentation**
- [p] research built-ins
- [p] research interactive mode
- [p] research git workflow for working in a team
  - use git cherry-pick to create a new branch to modularize pull requests.
- [ ] research `posix sh`
- [ ] research env arena design (prerequisite for export, unset, cd)
- [x] ~~compile documentation on signals~~
- [x] ~~research managing memory with multiple arenas, because there are actual multiple lifetimes~~
- [x] ~~research how to implement the variable content size for tokens in the context of expansion~~
- [x] ~~research how readline interacts with arenas and if it would even make sense to implement them~~
- [x] ~~what does a struct pointer dereference to, if its member is another struct. The first element of that struct?~~
- [-] ~~compile documentation on `flex` and `bison`~~
- [-] ~~compile documentation on `curses.h` and `term.h`~~

**implementation**
- [ ] prompt cwd -> refactor into its own reusable function, for the `cd` builtin.
- [p] create harness for automatic unit testing (tdd)
- [p] work on arenas - prepare prompt arena for `getcwd`
- [p] include the additional info in the make section.
- [p] draft the data structure and core architecture
- [ ] add push/pull mirroring on remote: could be hard, since i have added libft as a submodule
  - [ ] talk to nikita about it, whether he wants to work with it or not
- [ ] consider error handling according to posix
- [ ] implement mutable env unsure if arenas are the right choice here. this seems like exactly the opposiite of what they are useful for.
- [ ] implement parser reductions (grammar tables, reduce functions)
- [ ] resolve `$VAR` expansion tokens
- [ ] finish writing the readme file
- [x] ~~remove the statics from token.c, maybe into the ctx var, so i can actually test this properly in isolation.~~
- [x] ~~rework makefile to create/use separate folders (`src`, `include`, `bin`, `debug`, `test`)~~
- [x] ~~add `arena_grow` function to arena library~~
- [x] ~~add github remote, and github action workflow~~
- [x] ~~write a simple `flex` and `bison` based lexer and parser~~

**questions**
- [ ] is it necessary to track what a token was delimited by?
      seems to only be relevant for `io_number` and `io_location` which are optional
	  *see [2.10.1 Shell Grammar Lexical Conventions](https://pubs.opengroup.org/onlinepubs/9799919799/)*
- [ ] what does this mean in the context of Shell Grammar: "This formal syntax shall take precedence over the preceding text syntax description"
- [p] how i want to handle the strings when i pass them to execution, because i need an array of pointers to real C strings:
	1. reference the input and just add `\0` after each token delimiting.
	   problem: - i might not have the room to replace a discarded char with a `\0`
                - memmoving stuff around is costly
	2. copy over the input from the read_line to my arena after a token is delimited.
       problem: i already implemented the tokenizer this way. would be some refactoring
- [ ] what is this paragraph saying?
      *see [2.6 Word Expansions](https://pubs.opengroup.org/onlinepubs/9799919799/utilities/V3_chap02.html#tag_19_06)*
	```
	When the expansions in this section are performed other than in the context of preparing a command for execution, they shall be carried out in the current shell execution environment.
	```
- [x] ~~how do i treat single char operators that I don't need to handle, but that are part of operator strings: `&`~~

#### longterm

- [p] test automation for checking the repo before pushing to main (make + norminette for now)
- [p] create a resources repo
- [ ] create a dotfile repo
- [ ] create a README.md template
- [ ] create a script to automate initializing a git repo with remotes and templates
- [ ] create project website with git pages
- [ ] play around with `sh vi mode`
- [x] ~~migrate libft to its own repo and extract history from the repos it is in now~~

### Documentation

#### online
**readline**
- [GNU Readline Library](https://tiswww.cwru.edu/php/chet/readline/rltop.html)
  - [The GNU Readline Library](https://tiswww.cwru.edu/php/chet/readline/readline.html)
  - [The GNU History Library](https://tiswww.case.edu/php/chet/readline/history.html)
  - [The GNU Readline User Interface](https://tiswww.case.edu/php/chet/readline/rluserman.html)

**termcap**
- [The Termcap Manual](https://www.gnu.org/software/termutils/manual/termcap-1.3/html_mono/termcap.html)

**posix**
- [POSIX.1-2024](https://pubs.opengroup.org/onlinepubs/9799919799/)
  (same as "IEEE Std 1003.1-2024" and "The Open Group Standard Base Specifications, Issue 8")
  - [Shell & Utilities](https://pubs.opengroup.org/onlinepubs/9799919799/utilities/toc.html)
    - [Consequences of Shell Errors](https://pubs.opengroup.org/onlinepubs/9799919799/utilities/V3_chap02.html#tag_19_08_01)
  - [sh - shell, the standard command language interpreter](https://pubs.opengroup.org/onlinepubs/9799919799/utilities/sh.html)

**bash**
- [GNU Bash manual](https://www.gnu.org/software/bash/manual/bash.html)

**general**
- [Shell (Computing)](https://en.wikipedia.org/wiki/Shell_(computing))

**forgejo & codeberg**
- [Forgejo - Repository Mirrors](https://forgejo.org/docs/latest/user/repo-mirror)
- [Codeberg Pages](https://codeberg.page/)

#### local
**man pages**
- `man bash`
- `man readline`
- `man termios` - `tcgetattr`, `tcsetattr`

- `man 3 signal`
- `man 3 sigaction`
- `man 7 signal`
- `man 7 signal-safety`

- `man chdir`

- `man 5 core`

**pdfs**
- [Wang - Tutorial Flex Bison](../resources/wang-tutorial_flex_bison.pdf)
- [Levine - Flex & Bison](../resources/levine-flex&bison.pdf)
- [Aaby - Compiler Construction using Flex and Bison](../resources/aaby-compiler_construction_using_flex_and_bison.pdf)
