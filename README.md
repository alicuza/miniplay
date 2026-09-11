_This project has been created as part of the 42 curriculum by nribakov, sancuta._

# minishell

## Description

`minishell` is a miniature POSIX-like shell. It provides a REPL interface
with readline, a character-by-character POSIX tokenizer, an LALR(1) parser
scaffold, and arena-based memory management.

---

## Table of Contents

- [Description](#description)
- [Instructions](#instructions)
  - [Compilation](#compilation)
  - [Usage](#usage)
  - [Functionality](#functionality)
- [Known Limitations](#known-limitations)
- [Algorithm and Data Structure](#algorithm-and-data-structure)
  - [Architecture Overview](#architecture-overview)
  - [Core Data Structure](#core-data-structure)
  - [User Input](#user-input)
  - [Parsing](#parsing)
    - [Tokenization](#tokenization)
    - [Syntax Analysis](#syntax-analysis)
    - [Grammar](#grammar)
  - [Bash Specific Terms](#bash-specific-terms)
  - [Execution](#execution)
  - [Signals](#signals)
  - [Error Handling and Cleanup](#error-handling-and-cleanup)
- [Deviation from bash](#deviation-from-bash)
- [Resources](#resources)
  - [Documentation and References](#documentation-and-references)
  - [AI Usage](#ai-usage)

---

## Instructions

### Compilation

- `make` or `make all` - compiles `minishell`;
- `make clean` - removes object and dependency files;
- `make fclean` - additionally removes the binary, libs, docs, and logs;
- `make re` - recompiles the entire project from scratch;
- `make debug` - compiles with `-g -DDEBUG` for debugging;
- `make run` - builds and runs `./minishell`;
- `make run-debug` - builds and runs `./minishell-debug`;
- `make test` - builds both release and debug binaries, then runs the test suite;
- `make doc` - generates Doxygen man pages in `docs/` (requires `doxygen`);
- `make compile_flags.txt` - generates a clangd-compatible compilation database;

**Options:**
- `make [target] ARENA_SIZE=N` - overrides the arena initial capacity (default `64`);

**Dependencies:**
- libft (bundled)

### Usage

```bash
./minishell
```

### Functionality

- readline-based REPL with colored prompt (`user@hostname:cwd[exitcode]shni$ `)
- POSIX-conformant tokenizer (10 rules, quotes, expansions, operators)
- shift-only LALR(1) parser (reductions planned)
- builtins: `env`, `pwd`, `exit` (partial)
- arena-based memory management (no per-allocation tracking)
- interactive and non-interactive modes
- debug mode with `--scope=<flags>` and `--no_exec`

---

## TODO OR current Known Limitations

- **No pipelines, redirections, subshells, or chaining:** `|`, `<`, `>`, `<<`,
  `>>`, `&&`, `||`, `(`, `)` are lexed but never acted upon.
- **No heredoc:** `<<` is tokenized but the heredoc body reader is not
  implemented.
- **No signal handling:** `<signal.h>` is included but no handlers are
  registered. `SIGINT` and `SIGQUIT` are ignored by default.

---

## Algorithm and Data Structure

### Architecture Overview

The program state lives in a single `t_ctx` struct passed by pointer through
every stage. Memory is managed through typed arenas - there is no per-allocation
tracking or explicit `free` for individual tokens.

```
t_ctx
├── t_env          env            - linked list of environment key/value pairs
├── t_arena[5]     arena          - typed arenas
├── char          *read_line      - readline-allocated input (freed per iteration)
├── int            return_status  - exit code of the last command
├── bool           is_interactive - true when STDIN is a tty
└── (debug)        scope, no_exec - debug flags (only with -DDEBUG)
```

**Arenas:**

| Arena | Stride | Entry | Purpose |
|-------|--------|-------|---------|
| `AT_NONE` | - | - | Sentinel / untyped |
| `AT_PROMPT` | 1 | `char` | Prompt string buffer. Cleared per prompt. |
| `AT_STRING` | 1 | `char` | Token content, expanded strings. Cleared per parse. |
| `AT_STACK` | sizeof(t_symbol) | `t_symbol` | Shift-reduce parse stack. Cleared per parse. |
| `AT_CMD` | sizeof(t_node) | `t_node` | AST node pool. Cleared per parse. |

**Pipeline:**

```
read_line
    │
    ▼
  Lex ── character-by-character POSIX lexer ──► t_token
    │
    ▼
  Classify ── operator strings → grammar terminal
    │
    ▼
  Shift (AT_STACK) + Alloc (AT_CMD) ── WORD becomes t_node(NODE_ARG)
    │
    ▼
  Reduce ── pop AT_STACK frames, wire AT_CMD t_node pointers
    │
    ▼
  Execute ── walk AT_CMD root, fork/exec, apply redirects
    │
    ▼
  JIT Expand ── flagged tokens expanded inline during exec
```

Arena types use different strides: `AT_PROMPT` and `AT_STRING` are character
buffers (stride=1), `AT_STACK` has stride=`sizeof(t_symbol)`, `AT_CMD` has
stride=`sizeof(t_node)`.

### Core Data Structure

```c
typedef struct s_ctx
{
	t_env       env;
	t_arena     arena[AT_COUNT];
	char       *read_line;
	int         return_status;       // exit status of last command
	bool        is_interactive;      // true if STDIN is a tty
# ifdef DEBUG
	uint8_t     scope;               // debug scope flags SCOPE_*
	bool        no_exec;             // skip execution if set to true
# endif
}	t_ctx;

typedef struct s_symbol
{
	uint64_t      offset;            // offset into AT_STRING arena
	uint64_t      prev_symbol;       // arena index of logical predecessor
	uint64_t      next_frame;        // subshell chain (future use)
	uint64_t      next_pipeline;     // pipeline chain (future use)
	uint64_t      next_redir;        // command redirect chain (future use)
	uint64_t      next_arg;          // command arg chain (future use)
	uint64_t      next_cmd;          // pipeline level (future use)
	uint32_t      entry_state;       // parser state before this symbol
	uint32_t      flags;             // LEX_HAS_EXPANSION | LEX_HAS_QUOTES
	t_symbol_type type;              // SYM_TOKEN, SYM_OPERATOR, ...
}	t_symbol;

typedef struct s_parser_state
{
	uint32_t  cur_state;             // current parser state (always 0 - no reductions)
	uint64_t  arena_idx;             // physical index of most recently shifted symbol
	uint64_t  stack_idx;             // logical index (currently == arena_idx)
	t_token   lookahead;             // the current lookahead token
	uint8_t   flags;                 // PARSE_HERE_PENDING | PARSE_DONE
}	t_parser_state;

typedef struct s_lexer_state
{
	t_slice       token;             // position+length of token being built
	uint64_t      char_idx;          // index into read_line
	t_symbol_type type;              // SYM_TOKEN or SYM_OPERATOR
	uint8_t       flags;             // LEX_HAS_EXPANSION | LEX_HAS_QUOTES | ...
}	t_lexer_state;

typedef struct s_token
{
	uint64_t      offset;            // offset into AT_STRING arena
	uint32_t      flags;             // same flags as lexer_state
	t_symbol_type type;              // SYM_TOKEN, SYM_OPERATOR, ...
}	t_token;

// t_node — single AST node struct, allocated in AT_CMD.
// child = first/left child, next = sibling/right operand,
// redirs = redirect chain (commands only), offset = string offset.
typedef struct s_node
{
	t_node_type     type;
	struct s_node  *child;
	struct s_node  *next;
	struct s_node  *redirs;
	uint64_t        offset;
	uint32_t        flags;
	int             heredoc_fd;
}	t_node;
```

### User Input

Input is handled by a readline wrapper in `srcs/input.c`:

- **Interactive mode:** `isatty(STDIN_FILENO)` is checked once at startup.
  The prompt is built in the `AT_PROMPT` arena and includes color, user,
  hostname, cwd, and exit status:
  `\033[38;5;40muser@hostname:cwd[0]shni$ \033[0m`.
  Escape sequences are wrapped in `\001`/`\002` to prevent readline redisplay
  corruption. [TODO](go into more detail as to why)

- **Continuation prompt:** when `is_continuation` is true, default `"> "`
  is used (could add a proper `PS2` implementation instead).

- **Non-interactive mode** (3 approaches):
  1. `readline(NULL)` with `rl_outstream = stderr` — **current**.
     readline still mirrors input to stderr but not stdout. History is
     skipped when `!c->is_interactive`.
  2. `get_next_line()` line-by-line — **planned preferred**.
     Clean loop, no readline overhead in non-interactive mode.
  3. `read(2)` into `AT_INPUT` arena — **planned alternative**.
     Bulk-read stdin into a buffer, lexer scans forward preserving `\n`.
     Execution fires per `complete_command` reduction.

- **History:** in interactive mode, non-empty lines are added to readline
  history via `add_history()`. Not persisted between sessions.

### Parsing

Parsing follows a two-stage pipeline: **tokenization** (lexer) and **syntax
analysis** (parser). The lexer reads characters from `read_line` and produces
delimited tokens. The parser shifts those tokens onto a stack as `t_symbol`
nodes.

#### Tokenization

The lexer (in `srcs/lexer/`) implements the POSIX token recognition rules
from Section 2.3 of the Shell & Utilities volume. It is a character-by-character
state machine:

```
Lexer state tracks:
  token:      t_slice {pos, len}     current extent in read_line
  char_idx:   uint64_t               current read position
  type:       SYM_TOKEN / SYM_OPERATOR  //TODO: refined into the correct
                                       //      operator symbol during classify
  flags:      LEX_HAS_EXPANSION      token contains $ expansion
              LEX_HAS_QUOTES         token contains ' or " quotes
              LEX_IS_BUILDING        currently accumulating token
              LEX_IS_DELIMITED       token is complete, ready to shift
              LEX_NEEDS_INPUT        consumed all input, need more
```

The rules are applied in order (see POSIX 2.3 Token Recognition):

1. **EOI/EOF** — delimit current token if one exists
2. **Operator continuation** — if `prev_char` + `cur_char` form a longer
   operator (e.g. `<` + `<` → `<<`), extend the token
3. **Operator break** — if `prev_char` was an operator but `cur_char` cannot
   extend it, delimit the operator token
4. **Quotes** — `'` or `"` starts a quoted region; characters are added
   verbatim until the matching close quote
5. **Expansion** — `$` starts a variable reference; following valid `name_chars`
   are consumed
6. **Operator start** — unquoted `|`, `<`, `>`, `&`, `;`, `(`, `)` start a new
   operator token; delimit any existing token first
7. **Blank** — ` ` or `\t` delimits an existing token and is discarded
8. **Word continuation** — any other character extends a word token
9. **Comment** — `#` (when not quoted) discards until end of line
   (not required, optionally compiled)
10. **Default** — start a new word token

On delimiting, the token body is copied into `AT_STRING` arena and a `t_token`
is produced as the lookahead.

#### Syntax Analysis

The parser in `srcs/parser/` is a **shift-only LALR(1) parser**. The main loop
in `parse_input()`:

1. Create a zeroed `t_parser_state` and `t_lexer_state`
2. Set `parse.arena_idx = 1` (index 0 is the sentinel guard element)
3. Loop:
   - Call `get_lookahead()` — runs the lexer until a token is delimited
   - When a token is ready (`LEX_IS_DELIMITED`), call `shift_symbol()`
   - `shift_symbol()` allocates a `t_symbol` on `AT_STACK`, sets its offset,
     type, flags, entry_state (always 0), and `prev_symbol` pointing to the
     previous symbol
   - Update `stack_idx` and `arena_idx`
   - Repeat until EOF

No reductions are performed. The grammar rules (`t_rule[RULE_COUNT]`) and
reduce function pointers (`t_reduce`) are defined in `types.h` but never
populated. The parser creates a flat linked list of symbols — no non-terminal
nodes are synthesized.

**Planned:** The parser will perform LALR(1) shifts *and* reductions via
`reduce_by_rule()`, allocating `t_node` entries into `AT_CMD` on each shift
(for WORD tokens) and wiring them together during reduce. AT_STACK entries
become lightweight `t_frame` with `t_node *node_ref` instead of bare tokens.

#### Grammar
*see [2.10 Shell Grammar](https://pubs.opengroup.org/onlinepubs/9799919799/utilities/V3_chap02.html#tag_19_10)*

**Shell Grammar Lexical Conventions**
Lexing happens immediately following the `token` being delimited.

```
1.  if
        `cur_token` is `operator`
    do
        identify as corresponding `token_id`
```

```
2.  if  // we skip this
        `cur_token` is only `digits`
        && `delimiter` is `<` or `>`
    do
        identify as `IO_NUMBER`
```

```
3.  if  // we skip this
        `cur_token` is `{LOCATION}`
        && `delimiter` is `<` or `>`
    do
        identify as `IO_LOCATION`
```

```
4.  do
        identify as `TOKEN`
```

**Shell Grammar Rules**

```
1. (rule 2 - redirection to or from `filename`)
    1.1. tilde expansion
    1.2. parameter/variable expansion
    1.3. quote removal      // actually 5th expansion, but we don't handle
                            // command substitution or arithmetic expansion
    1.4. filename expansion
    1.5. word splitting     // if more than one word, bash errors with:
                            // `bash: $var: ambiguous redirect`
```

```
2. (rule 3 - redirection from `here-doc`)
    Quote removal of `WORD` after `DLESS` to determine `here-doc` `delimiter`
```

```
3. (rule 7 - assignment preceding command name)
    //TODO: continue writing
```

**documentation**
- [2.7 Redirections](https://pubs.opengroup.org/onlinepubs/9799919799/utilities/V3_chap02.html#tag_19_07)
- [2.10.2 Shell Grammar Rules](https://pubs.opengroup.org/onlinepubs/9799919799/utilities/V3_chap02.html#tag_19_10_02)
- [3.45 Blank Character](https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/V1_chap03.html#tag_03_45)

**Extended Backus-Naur Form (EBNF) Notation**

```ebnf
%token WORD NEWLINE PIPE LESS GREAT DLESS DGREAT AND_IF OR_IF OPAR CPAR

%start complete_commands

%%

complete_commands    : complete_commands newline_list complete_command
                     | complete_command
                     ;

complete_command     : list
                     ;

separator            : newline_list
                     ;

newline_list         : NEWLINE
                     | newline_list NEWLINE
                     ;

linebreak            : newline_list
                     | /* empty */
                     ;

list                 : and_or
                     ;

and_or               : pipeline
                     | and_or AND_IF linebreak pipeline
                     | and_or OR_IF  linebreak pipeline
                     ;

pipeline             : pipe_sequence
                     ;

pipe_sequence        : command
                     | pipe_sequence PIPE linebreak command
                     ;

command              : simple_command
                     | compound_command
                     | compound_command redirect_list
                     ;

compound_command     : subshell
                     ;

subshell             : OPAR compound_list CPAR
                     ;

compound_list        : linebreak term
                     | linebreak term separator
                     ;

term                 : term separator and_or
                     | and_or
                     ;

simple_command       : cmd_prefix cmd_word cmd_suffix
                     | cmd_prefix cmd_word
                     | cmd_prefix
                     | cmd_name cmd_suffix
                     | cmd_name
                     ;

cmd_name             : WORD
                     ;

cmd_word             : WORD
                     ;

cmd_prefix           : io_redirect
                     | cmd_prefix io_redirect
                     ;

cmd_suffix           : io_redirect
                     | cmd_suffix io_redirect
                     | WORD
                     | cmd_suffix WORD
                     ;

redirect_list        : io_redirect
                     | redirect_list io_redirect
                     ;

io_redirect          : io_file
                     | io_here
                     ;

io_file              : LESS filename
                     | GREAT filename
                     | DGREAT filename
                     ;

filename             : WORD
                     ;

io_here              : DLESS here_end
                     ;

here_end             : WORD
                     ;

%%
```

### Bash Specific Terms

**bash grammar**
the following chars don't have an explicit token they get assigned to in the
grammar.

```
'&' '|' '(' ')' '<' '>'
```

being single char tokens, they can just represent themselves?

**metacharacters** - *see [2. Definitions - metacharacter](https://www.gnu.org/software/bash/manual/bash.html#index-metacharacter)*
characters that separate words when unquoted.

```
'|', '&', ';', '(', ')', '<', '>'
' ', '\t', '\n'
```

**operators** - *see [2. Definitions - operator](https://www.gnu.org/software/bash/manual/bash.html#index-operator_002c-shell)*
`operator`s contain at least one unquoted `metacharacter`

**blanks** - *see [2. Definitions](https://www.gnu.org/software/bash/manual/bash.html#Definitions)*

```
' ', '\t'
```

**control operators** - *see [2. Definitions - control operator](https://www.gnu.org/software/bash/manual/bash.html#index-control-operator)*

```
'||', '&&', '(', ')'
'&', ';', ';;', ';&', ';;&', '|', '|&' - not implemented
```

**redirection operators** - *see [3.6 Redirections](https://www.gnu.org/software/bash/manual/bash.html#Redirections)*

```
'<', '>', '>>'
'<<' // here-doc
```

**name** - *see [2. Definitions - name](https://www.gnu.org/software/bash/manual/bash.html#index-name)*

`word`s that conform to the following format: `[a-zA-Z_][a-zA-Z0-9_]`

### Execution

[TODO]()
currently limited to a flat walk of the symbol stack in `token_processor.c`.
Will walk AT_CMD AST via `t_node` pointers.

### Signals

[TODO]()
currently no signal handling. `<signal.h>` is included in `minishell.h`.

### Error Handling and Cleanup

[TODO]()
cleanup is incomplete.

---

## Deviation from bash

- `ASSIGNMENT_WORD`   potentially necessary — can be handled in `export`
                       as a builtin with string parsing instead of a dedicated
                       token type
- `IO_NUMBER`         fd-specific redirects like `2>` are not mentioned by the
                       subject and not required per campus consensus
- `;`, `!`, `\`, `~`  not required, but interesting to consider
- `~`                 used in the dynamic prompt
- `#`                 comments are implemented

---

## Resources

### Documentation and References

### AI Usage

AI tools were used mainly for interactive rubber ducking and searching for
information.
