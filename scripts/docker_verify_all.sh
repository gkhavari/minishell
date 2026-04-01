#!/usr/bin/env bash
# Run inside 42-Docker-DevEnv from repo root (/app): full build, LeaYeh tester m+ne,
# norminette on includes/libft/src, then Valgrind + funcheck (builtins/pipes/redirs stdin).
set -euo pipefail
cd /app
export GH_BRANCH=IGNORE

echo "========== 1) Build (make re CC=clang-12) =========="
make re CC=clang-12

TESTER_DIR=/root/42_minishell_tester
if [ ! -d "$TESTER_DIR/cmds" ]; then
	echo "========== Clone 42_minishell_tester =========="
	git clone --depth 1 https://github.com/LeaYeh/42_minishell_tester.git "$TESTER_DIR"
	chmod +x "$TESTER_DIR/tester.sh"
fi

echo "========== 2) tester.sh m (mandatory) =========="
"$TESTER_DIR/tester.sh" --no-update m

echo "========== 3) tester.sh ne (empty env) =========="
"$TESTER_DIR/tester.sh" --no-update ne

echo "========== 4) norminette (includes, libft, src) =========="
norminette -R CheckForbiddenSourceHeader -R CheckDefine includes libft src

echo "========== 5) debug build + Valgrind + funcheck (shared_stress_input) =========="
make debug CC=clang-12
bash /app/scripts/run_stress_memcheck.sh /app/scripts/shared_stress_input.txt

echo "========== ALL CHECKS PASSED =========="
