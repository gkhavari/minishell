#!/usr/bin/env bash
# Push the modified minishell_tester (RUNDIR + MINISHELL_PATH defaults) to your GitHub.
# Create the repo on GitHub first (e.g. github.com/cozygarage/42_minishell_tester), then:
#
#   ./scripts/push_tester_fork.sh https://github.com/YOUR_USER/42_minishell_tester.git
#
# Or set COZYGARAGE_TESTER_REPO and run with no args.
set -e
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
source "$SCRIPT_DIR/ensure_deps.sh"
TESTER_DIR="$REPO_ROOT/minishell_tester"
REMOTE_NAME="cozygarage"

FORK_URL="${1:-${COZYGARAGE_TESTER_REPO}}"
if [[ -z "$FORK_URL" ]]; then
  echo "Usage: $0 <fork-repo-url>"
  echo "   or: COZYGARAGE_TESTER_REPO=<url> $0"
  echo "Example: $0 https://github.com/cozygarage/42_minishell_tester.git"
  exit 1
fi

if [[ ! -d "$TESTER_DIR" ]]; then
  echo "minishell_tester/ not found. Run ./scripts/run_minishell_tester.sh m once to clone it."
  exit 1
fi

cd "$TESTER_DIR"
if ! git rev-parse --is-inside-work-tree &>/dev/null; then
  echo "minishell_tester/ is not a git repo."
  exit 1
fi
# Remind to commit modifications so the fork has RUNDIR + MINISHELL_PATH fixes
if git status --short tester.sh 2>/dev/null | grep -q .; then
  echo "Warning: uncommitted changes in tester.sh. Commit so your fork has the fix:"
  echo "  cd minishell_tester && git add tester.sh && git commit -m 'Fix RUNDIR for in-repo clone; default MINISHELL_PATH=../'"
  echo ""
fi

if git remote get-url "$REMOTE_NAME" &>/dev/null; then
  git remote set-url "$REMOTE_NAME" "$FORK_URL"
else
  git remote add "$REMOTE_NAME" "$FORK_URL"
fi
echo "Pushing to $FORK_URL (remote: $REMOTE_NAME)..."
BRANCH="$(git branch --show-current)"
git push -u "$REMOTE_NAME" "$BRANCH"
echo "Done. Clone from your fork with: COZYGARAGE_TESTER_REPO=$FORK_URL ./scripts/run_minishell_tester.sh m"
