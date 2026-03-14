# 42_minishell_tester — Mandatory Failures Summary

This document summarizes **which tests failed** in the 42_minishell_tester mandatory suite and **in which situation** (STD_OUT, STD_ERR, or EXIT_CODE). Use it to prioritize fixes.

**Run:** `cd "$HOME/42_minishell_tester"` then run the tester (e.g. `bash tester.sh m`).

**Overall (from your run):**
- **Total tests:** 941  
- **Passed:** 865  
- **Leaking:** 0  
- **STD_OUT failures:** 31  
- **STD_ERR failures:** 50  
- **EXIT_CODE failures:** 33  

---

## 1. COMPARE PARSING (`0_compare_parsing.sh`)

| Case | STD_OUT | STD_ERR | EXIT_CODE | Script line |
|------|---------|---------|-----------|-------------|
| 24–30 | ❌ | ✅ | ✅ | 52, 54, 56, 58, 60, 62, 64 |
| 65 | ❌ | ❌ | ✅ | 134 |
| 72 | ❌ | ❌ | ✅ | 148 |

**Summary:** **9** failures. **7** are STD_OUT only (lines 52–64, even lines). **2** are STD_OUT + STD_ERR (lines 134, 148). No EXIT_CODE failures in this section.

**Likely cause:** Parsing/expansion or output format differs from bash for specific compare-parsing commands (e.g. quoted or escaped strings). The two STD_OUT+STD_ERR cases may be syntax/error message wording.

---

## 2. PARSING HELL (`10_parsing_hell.sh`)

| Case | STD_OUT | STD_ERR | EXIT_CODE | Script line |
|------|---------|---------|-----------|-------------|
| 12 | ❌ | ❌ | ✅ | 32 |
| 14 | ❌ | ❌ | ✅ | 40 |
| 15 | ❌ | ❌ | ✅ | 45 |
| 123 | ✅ | ✅ | ❌ minishell(2) bash(127) | 319 |
| 124 | ✅ | ❌ | ❌ minishell(1) bash(0) | 321 |

**Summary:** **5** failures. **3** are STD_OUT + STD_ERR (lines 32, 40, 45). **1** is EXIT_CODE only: we return **2** (syntax), bash returns **127** (command not found) — line 319. **1** is STD_ERR + EXIT_CODE: we return **1**, bash **0**; stderr differs — line 321.

**Likely cause:** Some parsing-hell input is interpreted as syntax error (2) or error (1) by us but as “command not found” (127) or success (0) by bash; or our error message text differs.

---

## 3. BUILTINS (`1_builtins.sh`)

| Failure type | Count | Script lines (examples) | Notes |
|--------------|-------|--------------------------|--------|
| **STD_OUT only** | 8 | 59, 61, 63, 65, 67, 69, 71, 390 | Output differs from bash. |
| **STD_OUT + STD_ERR** | 2 | 127, 142 | Output and error message both differ. |
| **STD_ERR only** | 3 | 105, 107, 239 | Error message wording differs. |
| **STD_ERR + EXIT_CODE** | many | 102, 200, 207–216, 224, 225–230, 235–236, 240–241, 249–251, 518, 520, 522, 524, 526, 528, 530, 532, 534, 536, 538, 554, 556, 558, 560, 562, 564, 574, 576, 578, 580, 584, 586, 602, 604, 606, 490 | We return **1**, bash **0** (or we 1, bash 127 in one case). |
| **EXIT_CODE only** | 1 | 360 | minishell(1) vs bash(127). |

**Summary:** Most builtin failures are **STD_ERR + EXIT_CODE**: we exit with **1** where bash exits with **0** (or 127 in one case). Often this means we print an error (or different text) and set exit 1 for a case where bash succeeds or fails with another code.

**Likely causes:**  
- **export** / **unset** / **env** output format or when we return 1 vs 0.  
- **cd** with extra args or edge cases: we may error (1) where bash succeeds (0).  
- One test (360): we 1, bash 127 — possibly “command not found” vs our generic error.

---

## 4. PIPELINES (`1_pipelines.sh`)

| Case | STD_OUT | STD_ERR | EXIT_CODE | Script line |
|------|---------|---------|-----------|-------------|
| 1–2 | ✅ | ❌ | ✅ | 4, 6 |
| 15–16 | ❌ | ✅ | ✅ | 32, 34 |
| 25–26, 27–28, 30 | ✅ | ❌ | ✅ | 55, 59, 68, 84, 110 |

**Summary:** **2** STD_OUT failures (lines 32, 34). **6** STD_ERR-only failures (lines 4, 6, 55, 59, 68, 84, 110). No EXIT_CODE failures.

**Likely cause:** Pipeline stdout in two cases differs from bash. In several others we print something to stderr (e.g. prompt or message) that bash does not.

---

## 5. REDIRS (`1_redirs.sh`)

**Summary:** **0** failures in this run. All 91 cases passed.

---

## 6. SCMD (`1_scmds.sh`)

| Case | STD_OUT | STD_ERR | EXIT_CODE | Script line |
|------|---------|---------|-----------|-------------|
| 7 | ❌ | ✅ | ✅ | 19 |

**Summary:** **1** failure: STD_OUT only at line 19.

**Likely cause:** One specific simple-command case produces different stdout (e.g. quoting, expansion, or redirection result).

---

## 7. VARIABLES (`1_variables.sh`)

**Summary:** **0** failures in this run.

---

## 8. CORRECTION (`2_correction.sh`)

| Case | STD_OUT | STD_ERR | EXIT_CODE | Script line |
|------|---------|---------|-----------|-------------|
| 81 | ❌ | ✅ | ✅ | 171 |

**Summary:** **1** failure: STD_OUT only at line 171.

**Likely cause:** One correction test expects different stdout (format or content).

---

## 9. PATH FAILS (`2_path_check.sh`)

| Case | STD_OUT | STD_ERR | EXIT_CODE | Script line |
|------|---------|---------|-----------|-------------|
| 11 | ✅ | ❌ | ✅ | 43 |
| 12 | ❌ | ✅ | ✅ | 47 |

**Summary:** **2** failures: one STD_ERR only (line 43), one STD_OUT only (line 47).

**Likely cause:** Path-check tests — we may print a different error message (43) or different stdout (47) than bash for a missing/wrong path.

---

## 10. SYNTAX ERRORS (`8_syntax_errors.sh`)

| Case | STD_OUT | STD_ERR | EXIT_CODE | Script line |
|------|---------|---------|-----------|-------------|
| 42 | ✅ | ✅ | ❌ minishell(2) bash(0) | 100 |

**Summary:** **1** failure: EXIT_CODE only. We return **2** (syntax error), bash returns **0**.

**Likely cause:** One “syntax error” test input is accepted by bash (e.g. valid in bash) but we treat it as syntax error and exit 2.

---

## 11. WILD (`9_go_wild.sh`)

| Case | STD_OUT | STD_ERR | EXIT_CODE | Script line |
|------|---------|---------|-----------|-------------|
| 2 | ✅ | ❌ | ✅ | 7 |
| 10 | ❌ | ❌ | ✅ | 30 |
| 13–15 | ❌ | ✅ | ✅ | 46, 49, 52 |

**Summary:** **5** failures: **1** STD_ERR only (7). **1** STD_OUT + STD_ERR (30). **3** STD_OUT only (46, 49, 52).

**Likely cause:** Wildcard/globbing: we don’t expand `*` (or do it differently). Bash expands; we may pass literal `*` or different output, and one case may produce different stderr (e.g. “no match” vs nothing).

---

## Summary by failure type

| Type | Approx. count | Where |
|------|----------------|--------|
| **STD_OUT only** | 31 | compare_parsing (7), parsing_hell (3), builtins (8), pipelines (2), scmds (1), correction (1), path_check (1), go_wild (4+) |
| **STD_ERR only** | 50 | builtins (many: wrong msg or we error vs bash success), pipelines (6), path_check (1), go_wild (1) |
| **EXIT_CODE only** | 33 | parsing_hell (1: we 2, bash 127; 1: we 1, bash 0), builtins (many: we 1, bash 0; one we 1, bash 127), syntax_errors (1: we 2, bash 0) |
| **STD_OUT + STD_ERR** | — | compare_parsing (2), parsing_hell (3), builtins (2), go_wild (1) |
| **STD_ERR + EXIT_CODE** | — | builtins (many: we 1, bash 0) |

---

## Situations at a glance

- **STD_OUT:** Minishell prints something different from bash (content or format). Focus: **compare_parsing** (lines 52–64), **parsing_hell** (32, 40, 45), **builtins** (59–71, 390), **pipelines** (32, 34), **scmds** (19), **correction** (171), **path_check** (47), **go_wild** (30, 46, 49, 52).
- **STD_ERR:** Minishell prints to stderr when bash doesn’t, or different text. Focus: **builtins** (many: export/env/cd edge cases), **pipelines** (extra stderr), **path_check** (43), **go_wild** (7), and parsing_hell/builtins combined with EXIT_CODE.
- **EXIT_CODE:** We return 1 or 2 where bash returns 0 or 127. Focus: **parsing_hell** (319: 2 vs 127; 321: 1 vs 0), **builtins** (1 vs 0 in many; 360: 1 vs 127), **syntax_errors** (100: 2 vs 0).

To fix in order: (1) EXIT_CODE cases where we should match bash (0 or 127). (2) STD_ERR wording/conditions so we don’t error where bash doesn’t. (3) STD_OUT for parsing, builtins, and wildcards (or document that wildcards are not implemented).
