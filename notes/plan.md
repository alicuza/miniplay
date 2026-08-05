# Parser Rewrite — Implementation Plan

Goal: replace the handwritten ad-hoc parsing in `shift_reduce.c` with a
bison-table-driven LALR parser with handler-based AST building, fix
trailing-newline-at-EOF acceptance via a grammar `program` wrapper, and
keep the **divert-based heredoc design** (early read at `io_here` reduction +
save/replay of tokens).

Milestones are ordered; each ends with a build/verify step.

---

## Phase 1 — Grammar edit + bison regeneration

`shni_grammar_reduced.y`:

1. `%start complete_commands` → `%start program`.
2. Insert before `complete_commands`:

   ```
   program
       : linebreak complete_commands linebreak
       | linebreak
       ;
   ```

3. Run `bison shni_grammar_reduced.y`.

**Gate — stop and reassess if anything other than this:**
- Exactly **1 shift/reduce conflict** in the `.output` (merged state after
  `complete_commands separator`: shift `list` vs reduce `linebreak: separator`;
  resolved as shift = correct behavior). It is benign: the separator is
  reduced as `linebreak` only when no `list` follows.
- `yyr1`/`yyr2`/`yyn` now show **45 rules**.
- `YYNTOKENS` stays **14** → `NTERM_OFFSET` stays **14**. New `YYFINAL`
  changes (re-copy).

`shni_grammar_reduced.tab.c` is a reference file (hand-copied tables); the
Makefile has no bison rule and does not compile it.

## Phase 2 — Renumbering (rules old N → N+2 for N ≥ 2)

`inc/types.h`:

- Insert `SYM_PROGRAM = 15` after `SYM_ACCEPT`; shift all nonterminals +1:

  | symbol | new | | symbol | new |
  |---|---|---|---|---|
  | SYM_ACCEPT | 14 | SYM_IO_REDIRECT | 29 |
  | SYM_PROGRAM | 15 | SYM_IO_FILE | 30 |
  | SYM_COMPLETE_COMMANDS | 16 | SYM_FILENAME | 31 |
  | SYM_LIST | 17 | SYM_IO_HERE | 32 |
  | SYM_PIPELINE | 18 | SYM_HERE_END | 33 |
  | SYM_COMMAND | 19 | SYM_SEPARATOR | 34 |
  | SYM_SUBSHELL | 20 | SYM_LINEBREAK | 35 |
  | SYM_COMPOUND_LIST | 21 | | |
  | SYM_TERM | 22 | | |
  | SYM_SIMPLE_COMMAND | 23 | | |
  | SYM_CMD_NAME | 24 | | |
  | SYM_CMD_WORD | 25 | | |
  | SYM_CMD_PREFIX | 26 | | |
  | SYM_CMD_SUFFIX | 27 | | |
  | SYM_REDIRECT_LIST | 28 | | |

- Rule table grows to **46 slots** (rules 1–45; slot 0 unused).

## Phase 3 — `shift_reduce.c` rewrite

Types (forward-declare the struct to break the typedef cycle):

```c
typedef struct s_rule t_rule;
typedef uint64_t (*t_reduce)(t_ctx *, t_parser_state *, t_rule *);
struct s_rule { t_reduce handler; uint32_t rhs_len; t_symbol_type lhs_type; };
```

- Replace the broken `rule_dispatch_first`/`rule_dispatch_second` (OOB local
  arrays, dangling returns, stray `;` at :139/:165) with one flat
  `static const t_rule` table, **each entry verified against the regenerated
  `yyr1`/`yyr2`**.
- `shift_reduce(c, parse)` returns `t_lalr_action` (SHIFT / REDUCE / ACCEPT /
  ERROR). `lookahead_type()` classifies the token (+ `PARSE_LOOKAHEAD_EOF` →
  END).
- `yypact[top]` is NINF → default reduce (`yydefact[top]`; `0` → LALR_ERROR).
- `yytable[yyact]` ≤ 0 → reduce by `-yytable[yyact]`; else shift.
- `shift()`: reaching YYFINAL → ACCEPT. Also guard on rule 1 (`$accept:
  program $end`) → ACCEPT.
- `reduce()`: capture `stack_at(rhs_len - 1)->token_idx` **before** `pop()`;
  handler → use its node idx; NULL handler → inherit `stack_at(0)->node_idx`;
  `push_nonterm(c, parse, lhs_type, node_idx, token_idx)` (this fixes the
  nonterm `token_idx`, required by `handle_here_body`'s reset).
- Add helpers: `stack_at(i)`, `node_alloc(...)`.

### Handler map (new rule numbers)

| Rule | Production | Handler |
|---|---|---|
| 1 | $accept: program $end | guard → ACCEPT |
| 2 | program: linebreak complete_commands linebreak | return `stack_at(1)->node_idx` |
| 3 | program: linebreak | NULL (node 0 — empty line) |
| 4 | complete_commands: complete_commands separator list | **chaining** (pastes) |
| 5 | complete_commands: list | NULL |
| 6 | list: pipeline | NULL |
| 7 | list: list AND_IF linebreak pipeline | FLAG_AND_IF |
| 8 | list: list OR_IF linebreak pipeline | FLAG_OR_IF |
| 9 | pipeline: command | NODE_PIPELINE creator |
| 10 | pipeline: pipeline PIPE linebreak command | pipeline append |
| 11–13 | command: simple_command / subshell / subshell redirect_list | NULL |
| 14 | subshell: OPAR compound_list CPAR | FLAG_SUBSHELL |
| 15–18 | compound_list / term rules | NULL |
| 19–23 | simple_command ×5 | NODE_COMMAND + arg/redir chain attach |
| 24 | cmd_name: WORD | NODE_ARG leaf |
| 25 | cmd_word: WORD | NODE_ARG leaf |
| 26 | cmd_prefix: io_redirect | append-chain |
| 27 | cmd_prefix: cmd_prefix io_redirect | append-chain |
| 28 | cmd_suffix: io_redirect | NULL |
| 29 | cmd_suffix: cmd_suffix io_redirect | append-chain |
| 30 | cmd_suffix: WORD | NODE_ARG leaf |
| 31 | cmd_suffix: cmd_suffix WORD | append-chain |
| 32 | redirect_list: io_redirect | NULL |
| 33 | redirect_list: redirect_list io_redirect | append-chain |
| 34 | io_redirect: io_file | NULL |
| 35 | io_redirect: io_here | NULL |
| 36 | io_file: LESS filename | REDIR_IN flag |
| 37 | io_file: GREAT filename | REDIR_OUT flag |
| 38 | io_file: DGREAT filename | REDIR_APPEND flag |
| 39 | filename: WORD | NODE_REDIR leaf |
| 40 | io_here: DLESS here_end | **heredoc owner** (see Phase 4) |
| 41 | here_end: WORD | NULL (inherits WORD's node 0) |
| 42 | separator: NEWLINE | NULL |
| 43 | separator: separator NEWLINE | NULL |
| 44 | linebreak: separator | NULL |
| 45 | linebreak: ε | `{NULL, 0, SYM_LINEBREAK}` |

## Phase 4 — Heredoc subsystem (divert design, kept)

- **`reduce_io_here` (rule 40)**: capture `parse->here.delim` from the
  here_end token (`stack_at(0)->token_idx`, quote-strip pos+1/len-2 on
  `TKN_HAS_QUOTES` — existing naive strip), set `PARSE_SAVE_TOKENS`, create
  placeholder NODE_REDIR (`REDIR_HERE | REDIR_HAS_QUOTES?` if delimiter token
  `TKN_HAS_QUOTES`, `arena_offset = 0`), return its idx.
  → `t_here_state` **moves into `t_parser_state`** (`parse->here`; resolves the
  parse_input.c:75 TODO). Drop the `here` param from `get_next_token` /
  `shift_reduce`.
- **`get_here_doc`**: on readline NULL → warning + append **empty BODY
  marker** + return TRUE (not `return false` at lex_heredoc.c:66) — keeps the
  replay bounded (no OOB past the token arena) and gives the placeholder a
  valid empty body.
- **`handle_here_body`**: write the body-token idx into the placeholder,
  **guarded by `top->type == SYM_IO_HERE`** (in the after-pass case the symbol
  is popped — skip the write, `arena_offset` stays 0 = empty body); keep the
  `token_idx` reset; set `PARSE_HAS_SAVED_TOKENS` (now always, since
  `get_here_doc` returns true).
- **`delimit_lex_here`**: scan the finished body slice for `$`/backtick → OR
  `TKN_HAS_EXPANSION` onto the body token (POSIX: expand iff delimiter
  unquoted — `REDIR_HAS_QUOTES` excludes at exec). Return the appended token
  idx. `REDIR_HAS_EXPAND` is dropped (scan is the fast-path).
- **`is_delim_line`**: also accept `'\0'` as line terminator.
- **Keep**: `PARSE_SAVE_TOKENS`, `PARSE_HAS_SAVED_TOKENS`, `PARSE_HERE_BODY`,
  `TKN_IS_HERE_BODY`, `handle_saved_tokens`, `get_here_doc` semantics.
- **Delete**: `try_reduce_symbol`, `PARSE_HERE_PENDING` (never set anywhere).

Multi-heredoc `cat <<a <<b` works because b's `io_here` reduces during the
replay of `<< b` at the `\n` lookahead and re-arms `PARSE_SAVE_TOKENS`.

## Phase 5 — Driver (`parse_input.c`) + main

- Status-gated loop: fetch token → divert intercept (**`PARSE_SAVE_TOKENS` &&
  current token is a `\n` operator** → HERE_BODY, unchanged shape; non-`\n`
  tokens dropped) → else `shift_reduce` → action switch.
- **EOI** (`PARSE_LOOKAHEAD_EOF`):
  1. If `PARSE_SAVE_TOKENS` pre-pass → **divert-at-EOF** (read body + replay
     saved tokens, so `cat <<EOF ls &&` + EOF reports a syntax error instead
     of false-accepting).
  2. **Final pass**: repeated `shift_reduce` with EOF lookahead until
     ACCEPT/ERROR.
  3. **After-pass**: if `PARSE_SAVE_TOKENS` got set during the pass
     (`cat <<EOF` + EOF) → read empty body + warning, guarded write.
- No EOF tolerance needed — the wrapper accepts `cat\n` + EOF and bare empty
  lines (fresh prompt).
- LALR_ERROR → `PARSE_ERROR` + `minishell: syntax error near unexpected token
  'X'` + break.
- Delete local `#define YYFINAL` (parse_input.c:14) and the YYFINAL state
  check (ACCEPT now comes from shift / rule-1 guard).
- `main.c:88`: exec only if `!(parse.flags & PARSE_ERROR)`; else
  `return_status = 2`.
- `input.c`: unchanged except the Ctrl-D fix — `get_user_input` returns NULL
  on readline NULL → clean shell exit.

## Phase 6 — Headers

- `minishell.h`: `PARSE_LOOKAHEAD_EOF 0x20` (next free after HERE_BODY 0x10),
  `REDIR_HAS_QUOTES 0x80` (next free after REDIR_APPEND 0x40), `RULE_COUNT`
  48 → **46**; decl updates: `shift_reduce`, `get_next_token` (no `here`
  param), `delimit_lex_here` (returns idx); remove `try_reduce_symbol` decl.
- `types.h`: SYM_PROGRAM + renumber, `t_here_state` inside `t_parser_state`,
  `t_rule`/`t_reduce`.

---

## Verification

1. Bison regen: `.output` shows exactly 1 shift/reduce conflict.
2. `make` builds clean.
3. Smoke tests (interactive + piped where possible):

| Case | Expected |
|---|---|
| `cat` | runs cat |
| `cat\nls` (bracketed paste) | runs both |
| empty line | fresh prompt (wrapper rule 3) |
| `cat\n` then Ctrl-D | accepted (trailing linebreak via wrapper) |
| `cat |` then Ctrl-D | syntax error |
| `cat <<EOF` with `$?` body | body expanded at exec |
| `cat <<'EOF'` | no expansion |
| `cat <<a <<b` | two divert cycles, both bodies read |
| `cat <<EOF ls` | body read before `ls` executes |
| `cat <<EOF ls &&` then Ctrl-D | syntax error via replay |
| Ctrl-D at prompt | clean exit |
