# Minishell — agent & contributor guide

This file is for **humans and coding agents** working on the repo. It records **how we develop**, **what rules bind the code**, and **how we keep quality** while improving design.

---

## 1. Authoritative documents

| Document | Role |
|----------|------|
| **[docs/minishellv10.md](docs/minishellv10.md)** | **Subject (v10.0):** mandatory scope, allowed functions, signals, builtins, what “good” means for grading. |
| **[docs/42_C_Codingguideline.md](docs/42_C_Codingguideline.md)** | **Norm 4.1-style rules:** line limits, naming, headers, forbidden constructs. **`norminette`** is the automated gate (run in Linux). |
| **[docs/MINISHELL_ARCHITECTURE.md](docs/MINISHELL_ARCHITECTURE.md)** | Data flow, modules, diagrams — update when structure changes. |
| **[docs/BEHAVIOR.md](docs/BEHAVIOR.md)** | Bash-aligned behavior and exit codes for testers. |
| **[docs/DATA_MODEL_AND_FUNCTIONS.md](docs/DATA_MODEL_AND_FUNCTIONS.md)** | Structs and function index — keep in sync after refactors. |
| **[includes/defines.h](includes/defines.h)** | **Shared constants and sentinels** (success/failure, exit codes, lexer/parser sentinels). Prefer names here over magic numbers. |

If behavior or structure disagrees with these, **fix code or docs** in small, reviewable steps.

---

## 2. Development setup (Windows host → Linux in Docker)

- **Host:** Windows (typical workflow: edit in Cursor/VS Code on the host).
- **Build & test target:** **Linux**, matching cluster-style evaluation — **Ubuntu 22.04**-based image, **`linux/amd64`**, via **`docker-compose.yml`**.
- **Container name:** `42-Docker-DevEnv` (service `run-container`).
- **Mount:** project root is mounted at **`/app`** in the container (`volumes: .:/app`).
- **Working directory in container:** `/app`.

### Typical commands (from host)

```bash
# Start dev container (if not running)
docker compose up -d

# Full rebuild inside container
docker compose run --rm run-container make re CC=cc

# Incremental build
docker compose run --rm run-container make CC=cc
```

### Tests (LeaYeh mandatory tester + optional Valgrind)

From the repo root on the host (requires Docker daemon and the container running or started by the script):

```bash
./scripts/run_minishell_tester.sh m    # mandatory (default)
./scripts/run_minishell_tester.sh vm   # mandatory + Valgrind
```

See the script header for modes (`ne`, `b`, `a`, `va`, …). Valgrind uses project **`minishell.supp`** for readline noise; **leaks in our code + libft still fail the run**.

**Norm (optional but recommended before push):**

```bash
docker compose run --rm run-container norminette -R CheckForbiddenSourceHeader -R CheckDefine includes libft src
```

---

## 3. Project goals (maintenance + redesign)

- **Maintain** passing mandatory tests (and Valgrind/funcheck where you use them).
- **Improve** structure and readability without changing graded behavior unless intentional and documented.
- **Design:** clear module boundaries, **SOLID** (especially single responsibility, dependency direction), **DRY** (one obvious place for each policy).
- **Predictability:** no silent failures; errors are **returned or surfaced** (`last_exit`, stderr, or abort helpers) according to layer rules below.

---

## 4. Error handling & `defines.h`

### Sentinels and exit-style values

- **`SUCCESS` / `FAILURE`** — general command/shell outcomes where applicable.
- **`MSH_OOM` (`-2`)** — **heap allocation failure** in the lexer/parser pipeline. **Not** the same as shell exit code `1`; it is an **internal propagate-up** sentinel (see comment in `defines.h`).
- **`PARSE_ERR` (`-1`)** — parse-related sentinel where used; do not confuse with `MSH_OOM`.
- **Shell exit codes** (`EXIT_SYNTAX_ERROR`, `EXIT_CMD_NOT_FOUND`, `EXIT_CMD_CANNOT_EXECUTE`, signal base **128+N**, **`EXIT_SIGINT`**, etc.) — use macros from **`defines.h`** or documented helpers; avoid bare `130`, `2`, `127` in scattered logic.

### OOM chaining (no silent leak)

1. **Deep helpers** (`append_char`, token creation, expansion helpers, etc.): on allocation failure, return **`MSH_OOM`** (or `NULL` only where the contract explicitly says the caller must handle it and free partial state).
2. **Do not** call **`clean_exit()`** from deep lexer/parser paths for OOM — that bypasses unified unwind of the current line’s allocations.
3. **Unwind frame** (e.g. **`tokenize_input`**, **`process_input`**): on **`MSH_OOM`**, call **`msh_lex_abort()`** (or equivalent) to free **`word`**, partial **`tokens`**, **`input`**, set **`last_exit`**, clear flags — **one place** owns “abort this line.”
4. **Child processes** may still use **`clean_exit`** after fork where the process must terminate immediately.

**Principle:** every allocation has a **clear owner**; on failure, ownership is either **transferred** to a success path or **freed** on the error path — **no orphan buffers**.

---

## 5. Ownership & debugging leaks / segfaults

When Valgrind or a crash points at a line:

1. **Who allocated?** (`malloc` / `ft_strdup` / `ft_calloc` / pipe, etc.)
2. **Who owns after return?** (caller frees vs callee steals vs list node owns `value`)
3. **Which path frees on error?** (early `return` branches must `free` partial work or delegate to `msh_lex_abort` / `free_*` helpers)

Prefer **short functions** with **explicit** “caller frees” / “takes ownership” comments at non-obvious boundaries. After refactors, update **`docs/DATA_MODEL_AND_FUNCTIONS.md`** so the index matches reality.

---

## 6. Macros in headers

- **`defines.h`:** **constants and simple, Norm-allowed macros** only (see **42** rules: no function-like macros that hide control flow if forbidden).
- **Do not** scatter magic numbers for exit status, lexer markers, or internal token prefixes — add or reuse a **`# define`** with a short comment.

---

## 7. Agent / contributor checklist (before merging)

- [ ] `make` (or `make re`) succeeds in **Docker** with **`cc`**, **`-Wall -Wextra -Werror`**.
- [ ] Mandatory tester still passes (or explain intentional behavior change + doc update).
- [ ] If touching allocation paths: **Valgrind** mode (`vm` / `va`) or project stress script where applicable.
- [ ] **norminette** clean on touched trees if Norm is a project requirement.
- [ ] **Docs** (`ARCHITECTURE`, `DATA_MODEL`, `BEHAVIOR`) updated if public contracts or file layout changed.
- [ ] **No silent OOM** — propagate **`MSH_OOM`** to the agreed unwind frame.

---

## 8. Optional: local reference

A **`.reference/`** directory (gitignored) may hold external repos for **reading only** (e.g. other shells). Do not copy large blocks without understanding; align behavior with **`BEHAVIOR.md`** and the subject.

---

*Keep this file accurate: when workflow or error policy changes, edit **AGENTS.md** in the same PR.*
