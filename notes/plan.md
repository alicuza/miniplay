# minishell — plan (post-4-Aug review & feature completion)

## 0. Done (removed)
Parser rewrite phases (grammar/bison tables, token renumbering, shift_reduce
rewrite, heredoc subsystem, driver, headers) — all committed. test_parse 37/37,
test_parse_tree 24/24.

## 1. Review of changes since 4 August
- Walk commits 5fcb28d → d2726d6 plus 4f71bef (heredoc repoint),
  0ac1546 (handler refactor), f66e242 (runner fix).
- Verify refactor equivalence: full `make test` (parse 37/37, tree 24/24).
- Heredoc edge cases: multi-heredoc `cat <<A <<B`, quoted delimiters.
- Runner dispatch: *search_execution* subprocess case.
- Open items: exec_list() TODO stub, builtin subprocess silence
  (pwd/cd/env/search_execution), rl_outstream = stderr echo in
  non-interactive mode (srcs/input.c:29).

## 2. Overly long functions
- 42 norm: ≤25 lines/func, ≤5 funcs/file, ≤80 cols. 882 norminette errors
  (detail in /tmp/opencode/norm.txt): 66× TOO_MANY_FUNCS, 121× LINE_TOO_LONG.
- Worst file: shift_reduce.c (732 lines, 217 errors, ~20+ statics).
  Split → shift_reduce.c + reduce_handlers.c + rule_tables.c; extract reduce()
  driver and shift_reduce() (lines ~705-732) first; then handle_here_body /
  get_here_doc, lookahead.c helpers.
- Gate: norminette per file after each move; tests after full split.

## 3. #ifdef DEBUG stripping script
- `#else` blocks exist — keep the #else branch:
  - srcs/parser/parse_input.c:149-151 (#ifndef DEBUG/#else/#endif)
  - srcs/lexer/lookahead.c:130-133 and 154-157 ((void)c;)
- ~50 #ifdef DEBUG sites across main.c, execute_non_builtin.c, pwd.c, env.c,
  lookahead.c, shift_reduce.c, parse_input.c, lex_heredoc.c, token_processor.c.
- tools/strip_debug.sh (awk): delete #ifdef DEBUG…#endif bodies; keep #else
  branches; run on a COPY (/tmp submission tree) so repo `make debug` works.
- Verify: stripped copy builds with `make` (non-DEBUG) + tests pass.
- Submission artifact = stripped copy; repo keeps DEBUG intact.

## 4. Execution
- exec_list() (token_processor.c:126) walks pipelines but executes nothing.
  Wire from AST: argv from NODE_ARG chain; redirections (open/dup2,
  REDIR_HERE body already in AT_STRING via attach_here_body — feed via
  temp file or pipe); pipes (fork/pipe/dup2, close fds); waitpid → status →
  c->return_status; subshell/&&/|| flags.
- Builtin dispatch exists (match_builtin, command_search_and_execution);
  init_command/build_command become obsolete once exec runs off NODE_COMMAND.
- Target: builtin subprocess tests green.

## 5. Expansion
- Only lexer detection exists (is_expansion_start, TKN_HAS_EXPANSION).
- $VAR via env_get (unset → empty/removed); $? via c->return_status
  (types.h:57); no expansion in single quotes; $ expands in double quotes;
  heredoc expands only with unquoted delimiter.
- Decide lex-time vs exec-time (exec-time recommended: expansion can change
  argv count); command name expansion must work.

## 6. Error handling
- Existing: report_parse_error (syntax), perror in cd/execute_non_builtin,
  exit_mem_issue, bare 127.
- Add: uniform `minishell: <ctx>: <strerror>` for command not found (127),
  permission denied (126), redirection/execve/fork/pipe failures; builtin
  error messages + exit codes; $? correctness (signals, not-found, builtins);
  &&/|| short-circuit depends on it; shell keeps running on all errors.

## 7. Signalling
- Nothing implemented (only <signal.h> include). At most one global
  (signal number).
- SIGINT (ctrl-C): interrupt line, new prompt; async-safe handler with
  rl_replace_line/rl_on_new_line/rl_redisplay. SIGQUIT (ctrl-\): ignore
  interactively. Children: reset SIG_DFL before execve; status 128+signo.
- ctrl-D (EOF): readline NULL → exit path.

## 8. Arena refactor (execution functions)
- Replace transient malloc/free with arena where lifetime ≤ one parse:
  build_command argv/pathname (token_processor.c:74/101), get_pathname joins,
  env_to_envp, ft_split_with_empty (PATH), get_path_canonical_form (:92),
  ft_split_key_value (export).
- Non-candidates: persistent env list (env_add/update/delete,
  add_env_defaults) — survives parses.
- Benefit: no per-parse leaks; verify with make test + valgrind.

## 9. Test coverage
Baseline: colleague tester LeaYeh/42_minishell_tester (42 Vienna fork of
zstenger93; install via curl installer → $HOME/42_minishell_tester, alias
mstest; clone already at /tmp/opencode/mstester).

- Method: pipes each command into ./minishell vs `bash --posix`, filters
  banner/prompt/exit messages, learns our program name and filters it,
  crash detection, valgrind leak + fd checks (`--track-fds=all`, `--no-stdfds`
  if we dup2 stdfds), failed cases + valgrind logs to mstest_output/.
- Mandatory categories (~1600 cases): compare_parsing 78, builtins cd 113 /
  echo 79 / env 4 / exit 68 / export 79 / pwd 6 / unset 77, pipelines 125,
  redirs 153, scmds 59, variables 100, correction 153, path_check 52,
  syntax_errors 101, parsing_hell 166, expansion 7, go_wild 37.
- Bonus: groups, operators, wildcards, subshell, correction, syntax_errors,
  go_wild. Plus crash/ (crash-resistance, e.g. `echo <<<> ok`) and no_env/
  (launch without environment), funcheck/leaks.sh.

Current state of our suite: test_parse 37 + test_parse_tree 24 (parser/AST
only); builtin pwd/cd/env + search_execution subprocess harnesses fail (exec
not wired). No coverage for builtin exit codes/export/unset/echo -n,
redirection behavior vs bash, pipelines e2e, expansion, syntax-error messages,
path search (127/126), crash resistance, no_env, valgrind.

Plan:
- Prereq: non-interactive EOF → exit, and no prompt/echo when stdin is not a
  tty, or the tester hangs/trashes stdout (with §7 signalling; rl_outstream
  issue from §1).
- After each feature phase run mstest and record per-category pass % here.
- Category mapping: redirs/pipelines/scmds → §4 execution; expansion/variables
  → §5; syntax_errors/correction/parsing_hell → §6 error handling;
  builtins_* + path_check → §4/§6; crash → parser robustness; no_env → env
  init; leaks.sh/valgrind → §8 arena refactor.
- Port high-value parse-level cases (syntax errors, crash cases, heredoc
  edges) into test/ so `make test` covers them without bash.
- Note: bash --posix differences (export format, redir word splitting;
  `--non-posix` flag available).
- Gate: mstest after §4, then after each phase — zero new failures in
  previously-passing categories.

## 10. Code style consistency & helper reuse
Audit done 5 Aug. Helpers created in the recent commits apply in 2 places
left; style divergences listed. TODO comments are NOT normalized now.

- Helper reuse:
  - token_processor.c:135-137 walks data.pipeline.next_idx by hand — the only
    manual chain walk left outside the parser; swap to node_next() (same as
    shift_reduce.c). Will grow again when exec_list is wired (§4).
  - get_offset_from_idx: KEEP (no change) — retained as the symmetric
    counterpart of get_idx_from_offset even though nothing calls it yet.
  - exit_mem_issue / is_expansion_start / arena helpers / print_* already used
    consistently everywhere; reduce_chain_append + stack/push/pop helpers are
    parser-internal by design (env's t_list append is a different domain).
- Style fixes:
  - #define EQUAL 0 duplicated in 6 files (token_processor.c, env_update.c,
    env_get.c, env_delete.c, builtin_export.c, unset.c): centralize.
  - GREEN/RESET duplicated in input.c + prompt.c: centralize or inline.
  - Hazardous macros in inc/env.h: `# define _ "_"` (only used as the `_`
    special-param key at execute_non_builtin.c:54) → rename SPECIAL_LAST_ARG;
    `# define n "n"` inside a DEBUG block → rename or remove.
  - `if(` / `return(` spacing (norm): get_pathname.c:30, execute_non_builtin.c:19
    and :55, prompt.c:55, env_to_envp.c:18/36/39/42/43, lookahead.c:36/120.
  - env_to_envp.c is off-style entirely (space indent, while(i <= 0)):
    reindent to tabs (worst standalone file, 106 norm errors).
  - Unify NULL-test style (if (!ptr) vs == NULL vs != NULL).
  - Error-message formats diverge (perror with embedded \n + prefix vs bare
    perror vs error_prefix var) — align with §6 error handling.
- Gate: norminette clean (excluding #ifdef DEBUG blocks and 42 headers) +
  make test green.

## 11. Git history cleanup
Rewrite the branch history: reorder commits, squash the wip steps, rename
commit messages, and set a single author. This is destructive — do it before
anything else and confirm scope/author first.

- Scope (to confirm): whole branch b232b63..f66e242 (parser bring-up + the
  3 new commits). Branch is pushed (`playground/lalr_parser` is ahead 4 of
  origin) — after rewriting, push with `--force-with-lease`.
- Safety: create backup ref `git branch backup/pre-rewrite` + rely on reflog;
  verify with `git fsck`; keep the old SHA map to diff trees.
- Tooling:
  - Reorder/squash/rename messages: `git rebase -i` (or `--rebase-merges`
    if merges are kept; prefer linearizing and dropping the side-branch
    merge commits 3bc27a2/9a1cdc9/940f4ea/8a43280/60a62cf since their
    content is already in the branch).
  - Author/committer rename (Nikita's + alicuza bot commits):
    `git filter-branch --env-filter` or `git filter-repo --mailmap`
    → single author, e.g. Stefan-Emanuel Ancuta <fainica24@gmail.com>
    (confirm the name/email).
- Proposed grouping (topological, oldest → newest):
  1. grammar + shift_reduce bring-up: squash b232b63, 7c3000e, 16beca0,
     0a0173e, 4ee3666, 208dc1c → one `feat: LALR parser core (bison tables +
     shift/reduce driver)`.
  2. parse_input refactors: squash f0d2cc1, 1c23002, 6be1cf7 → one
     `refactor: parse_input into 25-line functions`.
  3. debug traces: squash 7562105, abc041a, 789fb79, 69dc78d, 5b41cc6,
     7ed1c95, d2726d6 → one or two `debug:` commits.
  4. AST wiring: fdb9b1d, 5906d4d, 4e9b672 → `feat: wire AST chains,
     subshells, early complete_commands execution`.
  5. new work: keep 4f71bef (`fix: attach heredoc body to redir node`),
     0ac1546 (`refactor: collapse reduction handlers`), f66e242
     (`test: run search_execution as subprocesses`).
- Gate: after rewrite, `make test` green + tree diff of each commit vs the
  pre-rewrite SHA map.

## 12. Order
1. Git history cleanup (§11) — confirm scope/author, then rewrite
2. Review (§1) → 3. Strip script + norm cleanup (§2, §3) → 4. Style
consistency (§10) → 5. Execution (§4) → 6. Expansion (§5)
→ 7. Error handling (§6) → 8. Signalling (§7) → 9. Arena refactor (§8)
→ 10. Test coverage gate (§9) — run mstest after execution and each phase.

## 13. TODO checklist

Confirmed decisions: starting point `5fcb28d`; rewrite scope =
`5fcb28d..HEAD` (18 commits incl. `5fcb28d` itself); author →
Stefan-Emanuel Ancuta <fainica24@gmail.com>; review FIRST, rewrite scope
finalized from review, then the rest.

- [ ] Phase 0 — Review (§1)
  - [ ] Walk 18 commits `5fcb28d..HEAD` chronologically, classify each
        (pure refactor vs behavior change), note squash/rename intent
  - [ ] Verify refactor equivalence: full `make test` (parse 37/37,
        tree 24/24) + builtin/search_execution subprocess state
  - [ ] Heredoc edge cases: `cat <<A <<B`, quoted delimiters
  - [ ] Runner dispatch: `search_execution` subprocess case
  - [ ] Record open items (exec_list stub, builtin subprocess silence,
        rl_outstream in non-interactive mode)
  - [ ] Write review notes + confirm test baseline
- [ ] Phase 1 — Git history rewrite (§11)
  - [ ] Finalize grouping from review notes
  - [ ] Backup ref `backup/pre-rewrite` + reflog + `git fsck` + SHA map
  - [ ] Rewrite `5fcb28d..HEAD` (18 commits), drop stray merges
  - [ ] Author/committer → Stefan-Emanuel Ancuta <fainica24@gmail.com>
  - [ ] Gate: `make test` green + tree diff vs pre-rewrite map
  - [ ] Push `--force-with-lease`
- [ ] Phase 2 — Norm cleanup + strip script (§2, §3)
  - [ ] Split shift_reduce.c → reduce_handlers.c + rule_tables.c
  - [ ] Extract reduce()/shift_reduce() driver (lines ~705-732)
  - [ ] handle_here_body / get_here_doc, lookahead.c helpers
  - [ ] tools/strip_debug.sh (keep #else branches)
  - [ ] Gate: stripped /tmp copy builds with `make` + tests pass
- [ ] Phase 3 — Style consistency (§10)
  - [ ] Centralize EQUAL (6 files), GREEN/RESET, rename `_`/`n` macros
  - [ ] Fix `if(`/`return(` spacing (get_pathname, execute_non_builtin,
        prompt, env_to_envp, lookahead)
  - [ ] Reindent env_to_envp.c to tabs
  - [ ] Unify NULL-test style + error-message formats
  - [ ] Gate: norminette clean + `make test` green
- [ ] Phase 4 — Execution (§4)
  - [ ] Wire exec_list(): argv from NODE_ARG chain, redirs (open/dup2,
        heredoc via temp file/pipe), pipes, waitpid → c->return_status
  - [ ] Subshell / && / || flags
  - [ ] Obsolete init_command/build_command removal
  - [ ] Gate: builtin subprocess tests green
- [ ] Phase 5 — Expansion (§5)
  - [ ] $VAR via env_get, $? via c->return_status, exec-time expansion
  - [ ] Quote rules: single-quote no expansion, double-quote yes,
        heredoc only with unquoted delimiter
  - [ ] Command name expansion
- [ ] Phase 6 — Error handling (§6)
  - [ ] `minishell: <ctx>: <strerror>` for 127/126, redir/execve/fork/pipe
  - [ ] Builtin error messages + exit codes, $? correctness
  - [ ] Shell keeps running on all errors
- [ ] Phase 7 — Signalling (§7)
  - [ ] SIGINT: new prompt, async-safe handler (rl_replace_line etc.)
  - [ ] SIGQUIT ignored interactively; children SIG_DFL before execve
  - [ ] ctrl-D EOF exit path
- [ ] Phase 8 — Arena refactor (§8)
  - [ ] build_command argv/pathname, get_pathname joins, env_to_envp,
        ft_split_with_empty, get_path_canonical_form, ft_split_key_value
  - [ ] Verify with make test + valgrind (no per-parse leaks)
- [ ] Phase 9 — Test coverage gate (§9)
  - [ ] Prereq: non-interactive EOF exit + no prompt/echo when stdin
        not a tty
  - [ ] Run mstest after §4, then after each phase; record per-category
        pass % here; zero new failures in passing categories
  - [ ] Port high-value parse-level cases into test/
  - [ ] Final full `make test` + norminette + mstest sweep
