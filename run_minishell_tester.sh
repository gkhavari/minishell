#!/usr/bin/env bash
# Run 42_minishell_tester from project root. Usage:
#   ./run_minishell_tester.sh m      # mandatory
#   ./run_minishell_tester.sh vm     # mandatory + valgrind
#   ./run_minishell_tester.sh m b    # mandatory, builtins only
#   ./run_minishell_tester.sh ne     # no environment
# If minishell_tester/ is missing, clones the tester (upstream or COZYGARAGE_TESTER_REPO).
set -e
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

TESTER_REPO="${COZYGARAGE_TESTER_REPO:-https://github.com/zstenger93/42_minishell_tester.git}"
TESTER_DIR="minishell_tester"

if [[ ! -d "$TESTER_DIR" ]]; then
  echo "minishell_tester/ not found. Cloning tester..."
  git clone --depth 1 "$TESTER_REPO" "$TESTER_DIR"
  echo "Cloned into $TESTER_DIR/"
fi
if [[ ! -f minishell ]]; then
  echo "minishell binary not found. Run: make"
  exit 1
fi
export MINISHELL_PATH="$SCRIPT_DIR"
cd "$TESTER_DIR"
# If tester still has default RUNDIR, patch so it finds cmds/ next to the script
if grep -q 'RUNDIR=\$HOME/42_minishell_tester' tester.sh 2>/dev/null; then
  sed -i.bak "s|RUNDIR=\$HOME/42_minishell_tester|RUNDIR=\"\$(cd \"\$(dirname \"\${BASH_SOURCE[0]}\")\" \&\& pwd)\"|" tester.sh
fi
exec bash tester.sh "$@"
