#!/usr/bin/env bash
# Run 42_minishell_tester from project root. Usage (from repo root):
#   ./scripts/run_minishell_tester.sh m      # mandatory
#   ./scripts/run_minishell_tester.sh vm     # mandatory + valgrind
#   ./scripts/run_minishell_tester.sh m b    # mandatory, builtins only
#   ./scripts/run_minishell_tester.sh ne     # no environment
# If minishell_tester/ is missing, clones the tester (upstream or COZYGARAGE_TESTER_REPO).
# Set AUTO_INSTALL_DEPS=1 to try installing missing git (brew on macOS, apt-get on Linux).
set -e
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
cd "$REPO_ROOT"

# Optional: ensure git (and optionally install if AUTO_INSTALL_DEPS=1)
source "$SCRIPT_DIR/ensure_deps.sh"

TESTER_REPO="${COZYGARAGE_TESTER_REPO:-https://github.com/cozyGarage/42_minishell_tester.git}"
TESTER_DIR="$REPO_ROOT/minishell_tester"

if [[ ! -d "$TESTER_DIR" ]]; then
  echo "minishell_tester/ not found. Cloning tester..."
  git clone --depth 1 "$TESTER_REPO" "$TESTER_DIR"
  echo "Cloned into $TESTER_DIR/"
fi
if [[ ! -f "$REPO_ROOT/minishell" ]]; then
  echo "minishell binary not found. Run: make"
  exit 1
fi
export MINISHELL_PATH="$REPO_ROOT"
cd "$TESTER_DIR"
# If tester still has default RUNDIR, patch so it finds cmds/ next to the script (portable sed)
if grep -q 'RUNDIR=\$HOME/42_minishell_tester' tester.sh 2>/dev/null; then
  sed_i tester.sh "s|RUNDIR=\$HOME/42_minishell_tester|RUNDIR=\"\$(cd \"\$(dirname \"\${BASH_SOURCE[0]}\")\" \&\& pwd)\"|"
fi
exec bash tester.sh "$@"
