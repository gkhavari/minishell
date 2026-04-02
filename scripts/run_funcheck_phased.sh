#!/usr/bin/env bash
# Tiered memory-hardening helper (Docker/Linux, repo root).
#
# Important: funcheck without -a still runs malloc/calloc/… failure injection on
# every detected site; runtime grows with how much code your stdin exercises, not
# only with -a. Flag -a adds JSON function-fetch "allocations-not-freed" tracking.
#
# Workflow:
#   1) Valgrind (optional) — practical "still allocated at exit" signal.
#   2) funcheck -j -c (no -a) on a SMALL stdin (default smoke file) — quick matrix.
#   3) funcheck -jac only if Valgrind leaked, pass 2 failed, or FUNCHECK_ALWAYS_A=1.
#
# Env:
#   INPUT=file           stdin (default: scripts/funcheck_smoke_builtin_pipe_redir.txt)
#   SKIP_VALGRIND=1      skip Valgrind
#   FUNCHECK_ALWAYS_A=1  always run second pass with -a
#   STRESS_LOG_DIR=…     logs (default /tmp)
#
# Usage: bash scripts/run_funcheck_phased.sh
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
INPUT="${INPUT:-$ROOT_DIR/scripts/funcheck_smoke_builtin_pipe_redir.txt}"
IGNORE_FILE="$ROOT_DIR/readline_ignore.txt"
MSH="$ROOT_DIR/minishell"
LOG_DIR="${STRESS_LOG_DIR:-/tmp}"

if [ ! -f "$INPUT" ]; then
	echo "Error: input not found: $INPUT" >&2
	exit 1
fi
if [ ! -f "$MSH" ]; then
	echo "Error: build minishell first (e.g. make debug CC=clang-12)." >&2
	exit 1
fi
if [ ! -f "$IGNORE_FILE" ]; then
	echo "Error: missing $IGNORE_FILE" >&2
	exit 1
fi
if ! command -v funcheck >/dev/null 2>&1; then
	echo "Error: funcheck not in PATH." >&2
	exit 127
fi

readarray -t IGNORE_FUNCS < <(tr -d '\r' < "$IGNORE_FILE" | awk '!/^[[:space:]]*#/ && NF {print $1}')
if [ "${#IGNORE_FUNCS[@]}" -eq 0 ]; then
	echo "Error: no symbols in $IGNORE_FILE" >&2
	exit 1
fi

TMP_IN="$(mktemp)"
trap 'rm -f "$TMP_IN"' EXIT
tr -d '\r' < "$INPUT" > "$TMP_IN"

run_funcheck() {
	local with_alloc_track=$1
	local log=$2
	local -a args=(funcheck -j -c)
	if [ "$with_alloc_track" -eq 1 ]; then
		args+=( -a )
	fi
	local fn
	for fn in "${IGNORE_FUNCS[@]}"; do
		args+=( -i "$fn" )
	done
	args+=( "$MSH" )
	"${args[@]}" < "$TMP_IN" > "$log" 2>&1 || return $?
	return 0
}

VG_EC=0
if [ "${SKIP_VALGRIND:-0}" != "1" ]; then
	if ! command -v valgrind >/dev/null 2>&1; then
		echo "Warning: valgrind missing; set SKIP_VALGRIND=1 or install." >&2
	else
		VG_LOG="$LOG_DIR/funcheck_phased_valgrind.log"
		VG_OUT="$LOG_DIR/funcheck_phased_stdout.txt"
		echo "==> Valgrind (leak truth): log $VG_LOG"
		valgrind --trace-children=yes --track-fds=yes \
			--leak-check=full --show-leak-kinds=all \
			--errors-for-leak-kinds=all --error-exitcode=42 \
			--suppressions="$ROOT_DIR/readline.supp" \
			"$MSH" < "$TMP_IN" > "$VG_OUT" 2> "$VG_LOG" || VG_EC=$?
		VG_EC=${VG_EC:-0}
		echo "    Valgrind exit: $VG_EC (42 = leak flagged)"
	fi
else
	echo "==> Valgrind skipped (SKIP_VALGRIND=1)"
fi

FC1_LOG="$LOG_DIR/funcheck_phased_pass1.log"
echo "==> Funcheck pass 1 (-j -c, no -a): $FC1_LOG"
set +e
run_funcheck 0 "$FC1_LOG"
FC1_EC=$?
set -e
FC1_EC=${FC1_EC:-0}
echo "    Funcheck pass 1 exit: $FC1_EC"

RUN_A=0
if [ "${FUNCHECK_ALWAYS_A:-0}" = "1" ]; then
	RUN_A=1
	echo "==> Second pass: FUNCHECK_ALWAYS_A=1"
elif [ "$VG_EC" -eq 42 ]; then
	RUN_A=1
	echo "==> Second pass: Valgrind reported leaks (42)"
elif [ "$FC1_EC" -ne 0 ]; then
	RUN_A=1
	echo "==> Second pass: funcheck pass 1 failed"
fi

if [ "$RUN_A" -eq 1 ]; then
	FC2_LOG="$LOG_DIR/funcheck_phased_pass2_alloc_track.log"
	echo "==> Funcheck pass 2 (-jac): $FC2_LOG"
	set +e
	run_funcheck 1 "$FC2_LOG"
	FC2_EC=$?
	set -e
	FC2_EC=${FC2_EC:-0}
	echo "    Funcheck pass 2 exit: $FC2_EC"
	echo "--- pass 2 JSON head ---"
	head -n 25 "$FC2_LOG"
	if [ "$VG_EC" -ne 0 ] && [ "$VG_EC" -ne 42 ]; then
		exit "$VG_EC"
	fi
	if [ "$FC2_EC" -ne 0 ]; then
		exit "$FC2_EC"
	fi
	exit 0
fi

echo "--- pass 1 JSON head ---"
head -n 25 "$FC1_LOG"
if [ "$VG_EC" -ne 0 ] && [ "$VG_EC" -ne 42 ]; then
	exit "$VG_EC"
fi
if [ "$FC1_EC" -ne 0 ]; then
	exit "$FC1_EC"
fi
exit 0
