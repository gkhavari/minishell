# 42 Minishell Tester - Failure Tracking

This document tracks the current state of mandatory test failures from the [42_minishell_tester](https://github.com/cozyGarage/42_minishell_tester).

**Purpose:** Monitor test progress, identify regressions, and document known issues.

**Update protocol:** After any push to CI or local test run that changes the baseline, update the tables below.

---

## Current Baseline - fixvm branch (post-merge with main)

**Date:** 2026-03-31  
**Environment:** Docker Ubuntu 22.04 amd64, clang-12  
**Branch:** fixvm  
**Commit:** (post-merge with main b868856)  
**Test Run:** Local Docker via `./scripts/run_minishell_tester.sh m`

### Summary Statistics

| Mode | Total Tests | Passed | Failed | Leaks | Crashes | Pass Rate |
|------|-------------|--------|--------|-------|---------|-----------|
| `m`  | 986         | **982** | 4     | N/A   | 0       | 99.59%    |
| `vm` | *(not yet run)* | - | - | - | - | - |
| `ne` | *(not yet run)* | - | - | - | - | - |
| `vne`| *(not yet run)* | - | - | - | - | - |

**Failure Breakdown (mode `m`):**
- **STD_OUT failures:** 0
- **STD_ERR failures:** 4
- **EXIT_CODE failures:** 0
- **CRASHES:** 0

**Note:** Previous baseline from AGENT.md §5 showed 983/986 with 3 failures. Current run shows 982/986 with 4 STD_ERR failures. Need to investigate the extra failure.

---

## Detailed Failure List

### Mode: `m` (Mandatory, no Valgrind)

| Test # | File:Line | STD_OUT | STD_ERR | EXIT_CODE | CRASH | Category | Notes |
|--------|-----------|---------|---------|-----------|-------|----------|-------|
| 44 | `1_pipelines.sh:180` | ✅ | ❌ | ✅ | ✅ | Race | 29-stage pipeline stderr ordering (OS scheduler) |
| 8  | `1_redirs.sh:18`     | ✅ | ❌ | ✅ | ✅ | Race? | New failure - needs investigation |
| 18 | `1_redirs.sh:38`     | ✅ | ❌ | ✅ | ✅ | Race | `cat <missing \| cat <infile` - two children race stderr |
| 93 | `2_correction.sh:221` | ✅ | ❌ | ✅ | ✅ | State | `mkdir bla_test` + `cd bla_test` state carryover |

**Race conditions** (tests 44, 18): These are timing-dependent failures where stderr from multiple child processes arrives in different orders between bash and minishell due to OS scheduler decisions. These are **non-fixable** without fundamentally changing process execution (which would break other behavior).

**State carryover** (test 93): The tester runs sequential blocks in the same temp directory. If a previous test creates `bla_test/`, this test's `mkdir` may fail differently than expected. This is a **tester sequencing issue**, not a shell bug.

**New failure** (test 8 at `1_redirs.sh:18`): This was not in the previous baseline. Needs investigation to determine if it's a regression from the main branch merge or another race condition.

---

## Historical Baseline Comparison

### Previous baseline (from AGENT.md §5, branch fixvm, 2026-03-31 pre-merge)

| Mode | Passed | Total | Leaks | Failures |
|------|--------|-------|-------|----------|
| `m`  | **983** | 986  | N/A   | 3 STD_ERR (ordering races) |
| `vm` | **985** | 986  | **0** | 1 STD_ERR (pipeline race) |
| `ne` | **44**  | 44   | N/A   | 0 ✅ |
| `vne`| **44**  | 44   | **0** | 0 ✅ |

**Change:** Went from 983/986 (3 failures) to 982/986 (4 failures). The new failure is `1_redirs.sh:18`. This needs root cause analysis.

---

## Known Behavior Deltas (Intentional, Not Bugs)

These are documented differences from bash that are **intentional** per project decisions:

1. **Non-numeric `exit` argument:**
   - Bash: exits with code **255**
   - Minishell: exits with code **2** (via `clean_exit(..., 2)`)
   - Source: `src/builtins/exit.c`
   - Tester impact: Some `EXIT_CODE` checks may fail if they test non-numeric exit args

2. **PATH caching (`had_path`):**
   - Startup snapshot of PATH existence affects `find_command_path()` behavior when PATH is later unset
   - May differ from bash in edge cases
   - Source: `src/core/init.c`, `had_path` flag

3. **Non-TTY input:**
   - Uses `read_line_stdin()` (not readline) for non-interactive input
   - Should match bash behavior for tester, but internal mechanism differs

---

## Action Items

### Immediate
- [ ] **Investigate `1_redirs.sh:18` failure** - new since merge with main
  - Check what command is being tested
  - Compare stderr output between bash and minishell
  - Determine if it's a race or a real regression

### Short-term
- [ ] Run `vm` mode to get Valgrind baseline (expect 0 leaks per AGENT.md)
- [ ] Run `ne` and `vne` modes for optional test baseline
- [ ] Document funcheck status (expect 135/135 malloc sites checked)

### Long-term
- [ ] Consider if state-carryover test (93) needs tester fix or shell workaround
- [ ] Review if non-numeric exit code delta should be changed to match bash (255)

---

## Testing Commands

**Run from repo root after `make`:**

```bash
# Mandatory tests (no Valgrind)
./scripts/run_minishell_tester.sh m

# Mandatory tests (with Valgrind)
./scripts/run_minishell_tester.sh vm

# Optional tests (no Valgrind)
./scripts/run_minishell_tester.sh ne

# Optional tests (with Valgrind)
./scripts/run_minishell_tester.sh vne
```

**Extract summary:**
```bash
# Get totals
grep -E "^(TOTAL TEST COUNT|TESTS PASSED)" /tmp/test_output.log

# Get failures
grep "❌" /tmp/test_output.log
```

---

## Update Log

| Date | Branch | Commit | Change | Notes |
|------|--------|--------|--------|-------|
| 2026-03-31 | fixvm | post-merge-main | 982/986 (4 failures) | New failure at 1_redirs.sh:18 |

---

*This document should be updated after every significant test run, especially before/after refactoring phases.*
