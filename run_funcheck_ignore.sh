#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"
IGNORE_FILE="$ROOT_DIR/readline_ignore.txt"
IGNORE_FUNCTIONS=""

if ! command -v funcheck >/dev/null 2>&1; then
    echo "Error: funcheck not found in PATH." >&2
    exit 127
fi

if [ ! -f "$IGNORE_FILE" ]; then
    echo "Error: ignore file not found: $IGNORE_FILE" >&2
    exit 1
fi

readarray -t IGNORE_FUNCS < <(grep -v '^[[:space:]]*#' "$IGNORE_FILE" | awk 'NF {print $1}')

if [ "${#IGNORE_FUNCS[@]}" -eq 0 ]; then
    echo "Error: no functions loaded from $IGNORE_FILE" >&2
    exit 1
fi

if [ "$#" -lt 1 ]; then
    echo "Usage: $0 <program> [args...]" >&2
    echo "Example: printf 'echo hi\nexit\n' | $0 ./minishell" >&2
    exit 1
fi

# Build the argument list: one `-i` per ignored function, then the program args
ARGS=(funcheck -ac)
for fn in "${IGNORE_FUNCS[@]}"; do
    ARGS+=(-i "$fn")
done
ARGS+=("$@")

exec "${ARGS[@]}"
