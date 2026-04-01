#!/usr/bin/env bash
# Run 42_minishell_tester inside the Ubuntu 22.04 amd64 Docker container.
# Source is live-mounted from Mac; build + test happen inside the Linux container.
#
# Usage: ./scripts/run_minishell_tester.sh [mode]
#
# Modes (passed directly to LeaYeh/42_minishell_tester tester.sh):
#   m    - mandatory tests only               (default, fastest)
#   vm   - mandatory tests + Valgrind
#   ne   - no-env tests
#   vne  - no-env tests + Valgrind
#   b    - bonus tests
#   vb   - bonus tests + Valgrind
#   a    - all tests
#   va   - all tests + Valgrind
#
# Valgrind flags used by the tester:
#   --leak-check=full --show-leak-kinds=all
#   --track-fds=all --trace-children=yes
#   --suppressions=minishell.supp  (suppresses readline/ncurses leaks)
#
# All leaks in minishell + libft code are treated as failures.

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
CONTAINER="42-Docker-DevEnv"
TESTER_DIR="/root/42_minishell_tester"
MODE="${1:-m}"

# Valgrind modes need -g symbols for useful output
is_valgrind_mode() {
    case "$1" in vm|vne|vb|va|vc) return 0 ;; esac
    return 1
}

# ── 1. Ensure container is running ────────────────────────────────────────────
if ! docker ps --format "{{.Names}}" | grep -q "^${CONTAINER}$"; then
    echo "==> Container not running — starting (docker compose up -d)..."
    docker compose -f "$PROJECT_DIR/docker-compose.yml" up -d
    sleep 1
fi

# ── 2. Clone tester if not already present ────────────────────────────────────
docker exec "$CONTAINER" bash -c "
    if [ ! -d '$TESTER_DIR' ]; then
        echo '==> Cloning LeaYeh/42_minishell_tester...'
        git clone https://github.com/LeaYeh/42_minishell_tester.git '$TESTER_DIR'
        chmod +x '$TESTER_DIR/tester.sh'
        echo '==> Tester ready.'
    fi
"

# ── 3. Build inside container ─────────────────────────────────────────────────
if is_valgrind_mode "$MODE"; then
    # Debug build: adds -g so Valgrind shows file:line in reports
    echo "==> Building with debug symbols (make debug CC=clang-12)..."
    BUILD_CMD="make debug CC=clang-12"
else
    echo "==> Building (make re CC=clang-12)..."
    BUILD_CMD="make re CC=clang-12"
fi

if ! docker exec "$CONTAINER" bash -c "cd /app && $BUILD_CMD"; then
    echo "Build FAILED"
    exit 1
fi

# ── 4. Run tester ─────────────────────────────────────────────────────────────
echo "==> Running tester (mode: $MODE)..."
docker exec \
    -e GH_BRANCH=IGNORE \
    "$CONTAINER" \
    bash -c "cd /app && '$TESTER_DIR/tester.sh' --no-update $MODE"
