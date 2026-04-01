#!/usr/bin/env bash
# Build a random batch of command lines from 42_minishell_tester mandatory scripts.
# Requires: tester clone at /root/42_minishell_tester (same as Docker setup).
# Usage: bash scripts/gen_stress_input_from_tester.sh [lines] [output_path]
# Example: bash scripts/gen_stress_input_from_tester.sh 50 /app/scripts/my_batch.txt
set -euo pipefail

LINES="${1:-50}"
OUT="${2:-$(cd "$(dirname "$0")" && pwd)/shared_stress_input.txt}"
TESTER_ROOT="${TESTER_ROOT:-/root/42_minishell_tester}"

if [ ! -d "$TESTER_ROOT/cmds/mand" ]; then
	echo "Error: $TESTER_ROOT/cmds/mand not found. Set TESTER_ROOT or clone the tester." >&2
	exit 1
fi

TMP="$(mktemp)"
trap 'rm -f "$TMP"' EXIT
sed '/^[[:space:]]*#/d;/^[[:space:]]*$/d' "$TESTER_ROOT/cmds/mand"/*.sh | shuf -n "$LINES" > "$TMP"
{
	cat "$TMP"
	echo exit
} | tr -d '\r' > "$OUT"

echo "Wrote $((LINES + 1)) lines (including exit) to $OUT"
