#!/usr/bin/env bash
# Optional dependency check for scripts in this repo. Source from other scripts:
#   SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"  # in scripts/
#   source "$SCRIPT_DIR/ensure_deps.sh"
# Or from tests/: source "$REPO_ROOT/scripts/ensure_deps.sh"
# Requires: git (for cloning testers). Tries to install if missing (macOS: brew, Linux: apt-get).
set -e

ensure_deps() {
  local missing=()
  command -v git &>/dev/null || missing+=(git)

  if [[ ${#missing[@]} -eq 0 ]]; then
    return 0
  fi

  # Try to install missing deps when AUTO_INSTALL_DEPS=1 (macOS: brew, Linux: apt-get with sudo)
  if [[ "${AUTO_INSTALL_DEPS:-0}" == "1" ]]; then
    case "$(uname -s)" in
      Darwin)
        if command -v brew &>/dev/null; then
          for cmd in "${missing[@]}"; do
            [[ "$cmd" == git ]] && brew install git 2>/dev/null || true
          done
        fi
        ;;
      Linux)
        if command -v apt-get &>/dev/null; then
          (sudo apt-get update -qq && sudo apt-get install -y git) 2>/dev/null || true
        fi
        ;;
    esac
  fi

  # Re-check after optional install
  missing=()
  command -v git &>/dev/null || missing+=(git)
  if [[ ${#missing[@]} -gt 0 ]]; then
    echo "Missing required command(s): ${missing[*]}" >&2
    case "$(uname -s)" in
      Darwin) echo "  macOS:  brew install git" >&2 ;;
      Linux)  echo "  Linux: sudo apt-get update && sudo apt-get install -y git" >&2 ;;
      *)      echo "  Install git for your OS." >&2 ;;
    esac
    return 1
  fi
  return 0
}

# Portable sed -i (macOS needs backup extension; Linux accepts either)
sed_i() {
  local file="$1"
  shift
  if [[ "$(uname -s)" == "Darwin" ]]; then
    sed -i.bak "$@" "$file" && rm -f "${file}.bak"
  else
    sed -i "$@" "$file"
  fi
}

# When sourced, run check; when executed, run check and exit on failure
if [[ "${BASH_SOURCE[0]}" != "${0}" ]]; then
  ensure_deps || exit 1
elif ! ensure_deps; then
  exit 1
fi
