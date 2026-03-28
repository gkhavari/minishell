# Residual Justification Report (2026-03-28)

## Scope

Second implementation slice focused on pipeline child scheduling / output ordering,
with the explicit target to reduce or eliminate tester mismatch at:

- `mand/1_pipelines.sh:180` (STDERR)

Repository constraints respected:

- Mandatory minishell scope only (`docs/minishellv10.md`)
- 42 Norm style constraints (`docs/42_C_Codingguideline.md`)
- No forbidden functions introduced

## Implemented Changes

### Code

- `src/executor/executor_pipeline.c`
  - Pipeline orchestration keeps a launch barrier path and uses ordered helper steps.
  - Barrier is enabled only when the pipeline has no redirections.
- `src/executor/executor_pipeline_steps.c` (new)
  - Isolated pipeline step helpers:
    - child fd setup
    - per-command pipe/fork step
    - barrier release helper
- `Makefile`
  - Added `src/executor/executor_pipeline_steps.c` to sources.

### Design Intent

- Reduce non-deterministic stderr ordering in very large all-not-found pipelines.
- Avoid semantic regressions in redirection pipelines (producer/consumer startup race).

## Validation Evidence

### Targeted tester

Command:

```bash
./minishell_tester/tester.sh --no-update -f ./minishell_tester/cmds/mand/1_pipelines.sh
```

Result after freeze:

- Total: 44
- Passed: 43
- Remaining: only `1_pipelines.sh:180` (STDERR order)

### Regression check

- A temporary regression at `1_pipelines.sh:126` occurred when barrier was applied to all pipelines.
- Fixed by gating barrier to non-redirection pipelines only.
- Re-test confirmed `1_pipelines.sh:126` passes again.

### Variance sampling (`1_pipelines.sh:180` command only)

12-run sample, hashed stderr outputs:

- `bash --posix`: 7 unique orderings
- `minishell`: 7 unique orderings

Prior minishell sample before this slice showed higher variance.

## Residuals And Justification

### Residual R1

- Case: `mand/1_pipelines.sh:180`
- Type: STDERR ordering only
- Observed: Same message set and count as bash (56 lines), different line order
- Cause: Concurrent children writing atomic lines to shared stderr; process scheduling
  still varies run-to-run

Justification:

- Attempting stronger serialization can diverge from bash-like concurrent behavior,
  and previously introduced redirection pipeline regressions.
- Current implementation is a low-risk compromise that reduces variance while
  preserving mandatory semantics.

## Freeze Decision

Freeze current behavior for this slice:

1. Keep launch barrier for non-redirection pipelines.
2. Keep barrier disabled for redirection pipelines.
3. Track `1_pipelines.sh:180` as a concurrency-order residual unless CI shows
   deterministic Linux mismatch beyond ordering.

## Next Phase Entry Condition

Proceed to refactor phase only after this slice is merged and CI re-checked,
with complexity reduction goals constrained by:

- 42 Norm
- Mandatory minishell subject scope
- No forbidden APIs
