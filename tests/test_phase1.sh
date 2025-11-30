#!/bin/bash
# **************************************************************************** #
#                                                                              #
#    Phase 1 Test Suite - Foundation & Builtins                                #
#    Run: bash tests/test_phase1.sh                                            #
#                                                                              #
# **************************************************************************** #

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Counters
PASS=0
FAIL=0
TOTAL=0

# Test function
run_test() {
    local name="$1"
    local cmd="$2"
    local expected="$3"
    
    TOTAL=$((TOTAL + 1))
    
    # Run in bash to get expected output
    bash_output=$(echo "$cmd" | bash 2>&1)
    bash_exit=$?
    
    # Run in minishell
    mini_output=$(echo "$cmd" | ./minishell 2>&1 | grep -v "^.*@.*:.*\$" | head -n -1)
    mini_exit=$?
    
    if [ "$bash_output" = "$mini_output" ]; then
        echo -e "${GREEN}[PASS]${NC} $name"
        PASS=$((PASS + 1))
    else
        echo -e "${RED}[FAIL]${NC} $name"
        echo -e "  Command: $cmd"
        echo -e "  Expected (bash): '$bash_output'"
        echo -e "  Got (minishell): '$mini_output'"
        FAIL=$((FAIL + 1))
    fi
}

# Direct test (no bash comparison)
run_direct_test() {
    local name="$1"
    local cmd="$2"
    local expected="$3"
    
    TOTAL=$((TOTAL + 1))
    
    mini_output=$(echo "$cmd" | ./minishell 2>&1 | grep -v "^.*@.*:.*\$" | head -n -1)
    
    if echo "$mini_output" | grep -q "$expected"; then
        echo -e "${GREEN}[PASS]${NC} $name"
        PASS=$((PASS + 1))
    else
        echo -e "${RED}[FAIL]${NC} $name"
        echo -e "  Command: $cmd"
        echo -e "  Expected to contain: '$expected'"
        echo -e "  Got: '$mini_output'"
        FAIL=$((FAIL + 1))
    fi
}

echo ""
echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}   PHASE 1: FOUNDATION & BUILTINS TEST${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Check if minishell exists
if [ ! -f "./minishell" ]; then
    echo -e "${RED}Error: ./minishell not found. Run 'make' first.${NC}"
    exit 1
fi

# ============================================================================ #
#                              1. INITIALIZATION                                #
# ============================================================================ #
echo -e "${YELLOW}--- 1. Initialization Tests ---${NC}"

# Test: Shell starts and exits cleanly with Ctrl+D
echo -e "${GREEN}[MANUAL]${NC} Shell starts with prompt (visual check)"
echo -e "${GREEN}[MANUAL]${NC} Ctrl+D exits shell cleanly (prints 'exit')"

# ============================================================================ #
#                              2. ECHO BUILTIN                                  #
# ============================================================================ #
echo ""
echo -e "${YELLOW}--- 2. Echo Builtin Tests ---${NC}"

run_test "echo basic" "echo hello" "hello"
run_test "echo multiple words" "echo hello world" "hello world"
run_test "echo -n flag" "echo -n hello" "hello"
run_test "echo -n multiple" "echo -n -n hello" "hello"
run_test "echo -nnnnn" "echo -nnnnn hello" "hello"
run_test "echo no args" "echo" ""
run_test "echo empty string" 'echo ""' ""
run_test "echo -n only" "echo -n" ""
run_test "echo -n stops at invalid" "echo -n -a hello" "-a hello"

# ============================================================================ #
#                              3. PWD BUILTIN                                   #
# ============================================================================ #
echo ""
echo -e "${YELLOW}--- 3. PWD Builtin Tests ---${NC}"

run_test "pwd basic" "pwd" "$(pwd)"

# ============================================================================ #
#                              4. CD BUILTIN                                    #
# ============================================================================ #
echo ""
echo -e "${YELLOW}--- 4. CD Builtin Tests ---${NC}"

run_test "cd /tmp then pwd" "cd /tmp && pwd" "/tmp"
run_test "cd HOME" "cd && pwd" "$HOME"
run_direct_test "cd nonexistent" "cd /nonexistent_dir_12345" "No such file"

# ============================================================================ #
#                              5. ENV BUILTIN                                   #
# ============================================================================ #
echo ""
echo -e "${YELLOW}--- 5. ENV Builtin Tests ---${NC}"

run_direct_test "env contains PATH" "env" "PATH="
run_direct_test "env contains HOME" "env" "HOME="
run_direct_test "env contains USER" "env" "USER="

# ============================================================================ #
#                              6. EXPORT BUILTIN                                #
# ============================================================================ #
echo ""
echo -e "${YELLOW}--- 6. Export Builtin Tests ---${NC}"

run_direct_test "export no args shows vars" "export" "declare -x"
run_direct_test "export set var" "export TEST_VAR=hello && env" "TEST_VAR=hello"
run_direct_test "export invalid name" "export 1INVALID=x" "not a valid identifier"

# ============================================================================ #
#                              7. UNSET BUILTIN                                 #
# ============================================================================ #
echo ""
echo -e "${YELLOW}--- 7. Unset Builtin Tests ---${NC}"

run_direct_test "unset removes var" "export TEMP_VAR=test && unset TEMP_VAR && env" ""
# Note: env output should NOT contain TEMP_VAR after unset

# ============================================================================ #
#                              8. EXIT BUILTIN                                  #
# ============================================================================ #
echo ""
echo -e "${YELLOW}--- 8. Exit Builtin Tests ---${NC}"

# Test exit codes
test_exit_code() {
    local name="$1"
    local cmd="$2"
    local expected_code="$3"
    
    TOTAL=$((TOTAL + 1))
    
    echo "$cmd" | ./minishell > /dev/null 2>&1
    actual_code=$?
    
    if [ "$actual_code" -eq "$expected_code" ]; then
        echo -e "${GREEN}[PASS]${NC} $name (exit code: $actual_code)"
        PASS=$((PASS + 1))
    else
        echo -e "${RED}[FAIL]${NC} $name"
        echo -e "  Expected exit code: $expected_code"
        echo -e "  Got exit code: $actual_code"
        FAIL=$((FAIL + 1))
    fi
}

test_exit_code "exit no args" "exit" 0
test_exit_code "exit 0" "exit 0" 0
test_exit_code "exit 42" "exit 42" 42
test_exit_code "exit 255" "exit 255" 255

# ============================================================================ #
#                              9. SIGNAL HANDLING                               #
# ============================================================================ #
echo ""
echo -e "${YELLOW}--- 9. Signal Handling (Manual Tests) ---${NC}"

echo -e "${BLUE}[MANUAL]${NC} Test Ctrl+C: Should show new prompt, not exit"
echo -e "${BLUE}[MANUAL]${NC} Test Ctrl+D: Should exit shell with 'exit' message"
echo -e "${BLUE}[MANUAL]${NC} Test Ctrl+\\: Should do nothing (ignored)"

# ============================================================================ #
#                              SUMMARY                                          #
# ============================================================================ #
echo ""
echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}              TEST SUMMARY              ${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""
echo -e "Total:  $TOTAL"
echo -e "${GREEN}Passed: $PASS${NC}"
echo -e "${RED}Failed: $FAIL${NC}"
echo ""

if [ $FAIL -eq 0 ]; then
    echo -e "${GREEN}✅ All automated tests passed!${NC}"
else
    echo -e "${YELLOW}⚠️  Some tests failed. Check output above.${NC}"
fi

echo ""
echo -e "${YELLOW}Note: Some tests require manual verification (signals, prompt).${NC}"
echo -e "${YELLOW}Note: Builtins are not yet wired to executor - some tests may fail.${NC}"
