#!/usr/bin/env bash
# Run Valgrind and funcheck on minishell with the SAME stdin file.
# Use in Docker (or Linux) from repo root:  bash scripts/run_stress_memcheck.sh
#
# Optional first argument: path to input file (default: scripts/shared_stress_input.txt)
# Optional: copy a batch from the tester, e.g.
#   sed '/^[[:space:]]*#/d;/^[[:space:]]*$/d' /root/42_minishell_tester/cmds/mand/*.sh | shuf -n 50 > /tmp/my_batch.txt
#   echo exit >> /tmp/my_batch.txt
#   bash scripts/run_stress_memcheck.sh /tmp/my_batch.txt
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
INPUT="${1:-$ROOT_DIR/scripts/shared_stress_input.txt}"
IGNORE_FILE="$ROOT_DIR/readline_ignore.txt"
MSH="$ROOT_DIR/minishell"

if [ ! -f "$INPUT" ]; then
	echo "Error: input file not found: $INPUT" >&2
	exit 1
fi
if [ ! -x "$MSH" ] && [ -f "$MSH" ]; then
	chmod +x "$MSH" 2>/dev/null || true
fi
if [ ! -f "$MSH" ]; then
	echo "Error: build minishell first (e.g. make debug CC=clang-12)." >&2
	exit 1
fi
if [ ! -f "$IGNORE_FILE" ]; then
	echo "Error: missing $IGNORE_FILE" >&2
	exit 1
fi
if ! command -v valgrind >/dev/null 2>&1; then
	echo "Error: valgrind not found in PATH." >&2
	exit 127
fi
if ! command -v funcheck >/dev/null 2>&1; then
	echo "Error: funcheck not found in PATH." >&2
	exit 127
fi

TMP_IN="$(mktemp)"
trap 'rm -f "$TMP_IN"' EXIT
tr -d '\r' < "$INPUT" > "$TMP_IN"

LOG_DIR="${STRESS_LOG_DIR:-/tmp}"
VG_LOG="$LOG_DIR/minishell_stress_valgrind.log"
VG_OUT="$LOG_DIR/minishell_stress_stdout.txt"

echo "==> Input: $INPUT ($(wc -l < "$TMP_IN") lines)"
echo "==> Valgrind log: $VG_LOG"
echo "==> Minishell stdout (captured): $VG_OUT"
echo ""

valgrind --trace-children=yes --track-fds=yes \
	--leak-check=full --show-leak-kinds=all \
	--errors-for-leak-kinds=all --error-exitcode=42 \
	"$MSH" < "$TMP_IN" > "$VG_OUT" 2> "$VG_LOG" || VG_EC=$?
VG_EC=${VG_EC:-0}
echo "Valgrind exit code: $VG_EC (42 = leak kind flagged with --errors-for-leak-kinds)"
echo "--- Last 40 lines of Valgrind report ---"
tail -n 40 "$VG_LOG"
echo ""

echo "==> Funcheck (same stdin as Valgrind; readline ignores from readline_ignore.txt)"
# Default: -j JSON (no interactive TTY; finishes reliably). Set FUNCHECK_JSON=0 for full UI.
readarray -t IGNORE_FUNCS < <(tr -d '\r' < "$IGNORE_FILE" | awk '!/^[[:space:]]*#/ && NF {print $1}')
if [ "${#IGNORE_FUNCS[@]}" -eq 0 ]; then
	echo "Error: no symbols in $IGNORE_FILE" >&2
	exit 1
fi
FUNCHECK_ARGS=(funcheck -ac)
if [ "${FUNCHECK_JSON:-1}" != "0" ]; then
	FUNCHECK_ARGS+=(-j)
fi
for fn in "${IGNORE_FUNCS[@]}"; do
	FUNCHECK_ARGS+=(-i "$fn")
done
FUNCHECK_ARGS+=("$MSH")
FC_LOG="$LOG_DIR/minishell_stress_funcheck.log"
echo "==> Funcheck log: $FC_LOG"
"${FUNCHECK_ARGS[@]}" < "$TMP_IN" > "$FC_LOG" 2>&1 || FC_EC=$?
FC_EC=${FC_EC:-0}
echo "Funcheck exit code: $FC_EC"
if [ "${FUNCHECK_JSON:-1}" != "0" ]; then
	echo "--- First 80 lines of funcheck JSON ---"
	head -n 80 "$FC_LOG"
else
	echo "--- Last 40 lines ---"
	tail -n 40 "$FC_LOG"
fi
echo ""
VG_EC=${VG_EC:-0}
FC_EC=${FC_EC:-0}
if [ "$VG_EC" -ne 0 ]; then
	exit "$VG_EC"
fi
if [ "$FC_EC" -ne 0 ]; then
	exit "$FC_EC"
fi
exit 0
