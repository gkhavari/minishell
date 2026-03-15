#!/usr/bin/env bash
# Run LucasKuhn/minishell_tester from the project repo root.
# Clones into tests/lucas_minishell_tester if missing. Usage:
#   ./tests/run_lucas_tester.sh
#   make -C tests test_lucas
# Set AUTO_INSTALL_DEPS=1 to try installing missing git (brew on macOS, apt-get on Linux).
set -e
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
cd "$REPO_ROOT"
source "$REPO_ROOT/scripts/ensure_deps.sh"

LUCAS_REPO="${LUCAS_MINISHELL_TESTER_REPO:-https://github.com/LucasKuhn/minishell_tester.git}"
LUCAS_DIR="$REPO_ROOT/tests/lucas_minishell_tester"

if [[ ! -d "$LUCAS_DIR" ]]; then
  echo "LucasKuhn minishell_tester not found. Cloning into $LUCAS_DIR ..."
  git clone --depth 1 "$LUCAS_REPO" "$LUCAS_DIR"
  echo "Cloned."
fi

if [[ ! -f "$REPO_ROOT/minishell" ]]; then
  echo "minishell binary not found. Run: make"
  exit 1
fi

# LucasKuhn tester expects MINISHELL_PATH = path to minishell binary (script defaults to ../minishell)
export MINISHELL_PATH="$REPO_ROOT/minishell"
cd "$LUCAS_DIR"
# Patch tester to use env MINISHELL_PATH so it finds our binary (we're in tests/lucas_minishell_tester)
if ! grep -q 'MINISHELL_PATH="\${MINISHELL_PATH' tester 2>/dev/null; then
  sed_i tester 's|^MINISHELL_PATH="../minishell"|MINISHELL_PATH="${MINISHELL_PATH:-../minishell}"|'
fi
if ! grep -q 'MINISHELL_PATH="\${MINISHELL_PATH' tester 2>/dev/null; then
  sed_i tester 's|^MINISHELL_PATH="../minishell_bonus"|MINISHELL_PATH="${MINISHELL_PATH:-../minishell}_bonus"|'
fi

# Inject our consolidated suite (phase1 + hardening + behavior) as "local" test file
if [[ -f "$REPO_ROOT/tests/local_tests" ]]; then
  cp "$REPO_ROOT/tests/local_tests" "$LUCAS_DIR/local"
  if ! grep -q '"local"' tester 2>/dev/null; then
    # Append "local" after "extras" (portable: use sed 'a\' + newline, not s/.../...\n/ which fails on BSD sed)
    sed_i tester $'/"extras"/a\\\n "local"'
  fi
fi

# Optional: bound invocations to avoid hangs (only if timeout/gtimeout exists; macOS has neither by default)
if [[ -f tester ]]; then
  if grep -q 'timeout 10s \$MINISHELL_PATH\|gtimeout 10s \$MINISHELL_PATH' tester 2>/dev/null; then
    : # already patched
  elif [[ "$(uname -s)" == "Darwin" ]] && command -v gtimeout &>/dev/null; then
    sed_i tester 's|\$MINISHELL_PATH|gtimeout 10s \$MINISHELL_PATH|g'
  elif command -v timeout &>/dev/null; then
    sed_i tester 's|\$MINISHELL_PATH|timeout 10s \$MINISHELL_PATH|g'
  fi
  # Prevent empty $PROMPT from breaking grep
  grep -q 'UNMATCHABLE_PROMPT' tester 2>/dev/null || \
    sed_i tester 's|grep -vF "\$PROMPT"|grep -vF "${PROMPT:-UNMATCHABLE_PROMPT}"|g'
fi

if [[ -x tester ]]; then
  exec ./tester "$@"
else
  exec bash tester "$@"
fi
