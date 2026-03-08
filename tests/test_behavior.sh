#!/bin/bash
# **************************************************************************** #
#  Behavior Test Suite - Covering Edge Cases documented in docs/BEHAVIOR.md    #
#  Run: bash tests/test_behavior.sh                                            #
# **************************************************************************** #

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

PASS=0
FAIL=0
TOTAL=0
SHELL_BIN="./minishell"

if [ ! -f "$SHELL_BIN" ]; then
    echo -e "${RED}Error: $SHELL_BIN not found. Run 'make' first.${NC}"
    exit 1
fi

# --------------------------------------------------------------------------- #
# Helper: compare minishell stdout to bash stdout
# --------------------------------------------------------------------------- #
run_cmp() {
    local name="$1"
    local input="$2"
    TOTAL=$((TOTAL + 1))

    bash_out=$(printf '%s\n' "$input" | bash 2>/dev/null)
    mini_out=$(printf '%s\n' "$input" | "$SHELL_BIN" 2>/dev/null)

    if [ "$bash_out" = "$mini_out" ]; then
        echo -e "${GREEN}[PASS]${NC} $name"
        PASS=$((PASS + 1))
    else
        echo -e "${RED}[FAIL]${NC} $name"
        echo "  input:    $input"
        echo "  bash:     $(printf '%s' "$bash_out" | head -3)"
        echo "  mini:     $(printf '%s' "$mini_out" | head -3)"
        FAIL=$((FAIL + 1))
    fi
}

run_cmp_direct() {
    local name="$1"
    local input="$2"
    local expected="$3"
    TOTAL=$((TOTAL + 1))

    mini_out=$(printf '%s\n' "$input" | "$SHELL_BIN" 2>/dev/null)

    if [ "$expected" = "$mini_out" ]; then
        echo -e "${GREEN}[PASS]${NC} $name"
        PASS=$((PASS + 1))
    else
        echo -e "${RED}[FAIL]${NC} $name"
        echo "  input:    $input"
        echo "  expected: $expected"
        echo "  mini:     $mini_out"
        FAIL=$((FAIL + 1))
    fi
}

# Helper: check exit code
run_exit() {
    local name="$1"
    local input="$2"
    local expected="$3"
    TOTAL=$((TOTAL + 1))

    printf '%s\n' "$input" | "$SHELL_BIN" > /dev/null 2>&1
    actual=$?

    if [ "$actual" -eq "$expected" ]; then
        echo -e "${GREEN}[PASS]${NC} $name (exit=$actual)"
        PASS=$((PASS + 1))
    else
        echo -e "${RED}[FAIL]${NC} $name (expected=$expected got=$actual)"
        FAIL=$((FAIL + 1))
    fi
}

# Helper: grep error output
run_grep() {
    local name="$1"
    local input="$2"
    local expected="$3"
    TOTAL=$((TOTAL + 1))

    mini_out=$(printf '%s\n' "$input" | "$SHELL_BIN" 2>&1)

    if echo "$mini_out" | grep -q "$expected"; then
        echo -e "${GREEN}[PASS]${NC} $name"
        PASS=$((PASS + 1))
    else
        echo -e "${RED}[FAIL]${NC} $name"
        echo "  input:    $input"
        echo "  expected: '$expected'"
        echo "  got:      '$mini_out'"
        FAIL=$((FAIL + 1))
    fi
}

echo ""
echo -e "${BLUE}========================================================${NC}"
echo -e "${BLUE}         BEHAVIOR SUITE — EDGE CASES                    ${NC}"
echo -e "${BLUE}========================================================${NC}"

# =========================================================================== #
echo ""
echo -e "${YELLOW}--- 1. Redirections Order & Multiple ---${NC}"
# =========================================================================== #
run_cmp "multiple out redirections" "echo success > /tmp/ms_b_out1 > /tmp/ms_b_out2
cat /tmp/ms_b_out2
rm -f /tmp/ms_b_out1 /tmp/ms_b_out2"

# In minishell, input redirections are validated before output files are created.
# If an input redirection fails, it skips creating any output files.
run_cmp_direct "redir mid fail output check" "echo test > /tmp/ms_b_out1 < /nonexistent_b_file > /tmp/ms_b_out2 2>/dev/null
[ -e /tmp/ms_b_out1 ] && echo out1_exists
[ -e /tmp/ms_b_out2 ] && echo out2_exists
rm -f /tmp/ms_b_out1 /tmp/ms_b_out2" ""

# =========================================================================== #
echo ""
echo -e "${YELLOW}--- 2. Builtins & Pipelining ---${NC}"
# =========================================================================== #
run_cmp "builtin in pipe" "cd /tmp | pwd"
run_cmp "exit code in pipe" "false | true
echo \$?"

# =========================================================================== #
echo ""
echo -e "${YELLOW}--- 3. Variables & Exit Status (\$?) ---${NC}"
# =========================================================================== #
run_cmp "exit status after failure" "nonexistent_b_cmd 2>/dev/null
echo \$?"

# =========================================================================== #
echo ""
echo -e "${YELLOW}--- 4. Exit Builtin Edge Cases ---${NC}"
# =========================================================================== #
run_exit "exit leading space" "exit   42" 42
run_exit "exit trailing space" "exit 42   " 42
run_exit "exit leading negative" "exit -1" 255
run_exit "exit leading zeros" "exit 00042" 42

# --------------------------------------------------------------------------- #
echo ""
echo -e "${BLUE}========================================================${NC}"
echo -e "${BLUE}                    SUMMARY                            ${NC}"
echo -e "${BLUE}========================================================${NC}"
echo -e "Total:  $TOTAL"
echo -e "${GREEN}Passed: $PASS${NC}"
echo -e "${RED}Failed: $FAIL${NC}"
echo ""

if [ $FAIL -eq 0 ]; then
    echo -e "${GREEN}✅ All behavior tests passed!${NC}"
    exit 0
else
    PCTG_OK=$((PASS * 100 / TOTAL))
    echo -e "${YELLOW}⚠️  $FAIL test(s) failed ($PCTG_OK% pass rate).${NC}"
    exit 1
fi
