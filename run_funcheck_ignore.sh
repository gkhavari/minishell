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

# Normalize potential CRLF and keep only non-comment symbols.
IGNORE_FUNCTIONS="$(
    tr -d '\r' < "$IGNORE_FILE" | awk '!/^[[:space:]]*#/ && NF {print $1}' | tr '\n' ' '
)"

if [ -z "$IGNORE_FUNCTIONS" ]; then
    echo "Error: no functions loaded from $IGNORE_FILE" >&2
    exit 1
fi

if [ "$#" -lt 1 ]; then
    echo "Usage: $0 <program> [args...]" >&2
    echo "Example: printf 'echo hi\nexit\n' | $0 ./minishell" >&2
    exit 1
fi

exec funcheck -a -i "$IGNORE_FUNCTIONS" "$@"
