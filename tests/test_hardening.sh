#!/bin/bash
# **************************************************************************** #
#  Hardening Test Suite - MANDATORY minishell features only                    #
#  Run: bash tests/test_hardening.sh                                           #
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

# Helper: grep minishell stderr+stdout for expected pattern
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

# Helper: no segfault (exit code 139)
no_crash() {
    local name="$1"
    local input="$2"
    TOTAL=$((TOTAL + 1))

    printf '%s\n' "$input" | "$SHELL_BIN" > /dev/null 2>&1
    code=$?

    if [ "$code" -eq 139 ]; then
        echo -e "${RED}[SEGFAULT]${NC} $name"
        FAIL=$((FAIL + 1))
    else
        echo -e "${GREEN}[NO CRASH]${NC} $name (exit=$code)"
        PASS=$((PASS + 1))
    fi
}

echo ""
echo -e "${BLUE}========================================================${NC}"
echo -e "${BLUE}         HARDENING SUITE — MANDATORY ONLY               ${NC}"
echo -e "${BLUE}========================================================${NC}"

# =========================================================================== #
echo ""
echo -e "${YELLOW}--- 1. Empty / Whitespace Input ---${NC}"
# =========================================================================== #
no_crash "empty string"                 ""
no_crash "spaces only"                  "   "
no_crash "tabs only"                    "	"
no_crash "many blank lines"             "$(printf '\n\n\n\n\n')"

# =========================================================================== #
echo ""
echo -e "${YELLOW}--- 2. Syntax Error Resilience (must not crash/segfault) ---${NC}"
# =========================================================================== #
no_crash "lone pipe"                    "|"
no_crash "double pipe"                  "||"
no_crash "pipe at end"                  "echo hi |"
no_crash "unclosed double quote"        'echo "hello'
no_crash "unclosed single quote"        "echo 'hello"
no_crash "redir no file (>)"            "echo hi >"
no_crash "multiple redirects no file"   ">> >> >>"
no_crash "heredoc no delim"             "cat <<"

# =========================================================================== #
echo ""
echo -e "${YELLOW}--- 3. Echo Builtin ---${NC}"
# =========================================================================== #
run_cmp  "echo basic"                   "echo hello"
run_cmp  "echo multiple words"          "echo hello world"
run_cmp  "echo -n"                      "echo -n hello"
run_cmp  "echo -nnn"                    "echo -nnn hello"
run_cmp  "echo -n stops at non-flag"    "echo -n -a hello"
run_cmp  "echo empty"                   "echo"
run_cmp  "echo empty string arg"        'echo ""'
run_cmp  "echo single quotes"           "echo 'hello world'"
run_cmp  "echo double quotes"           'echo "hello world"'

# =========================================================================== #
echo ""
echo -e "${YELLOW}--- 4. Variable Expansion ---${NC}"
# =========================================================================== #
run_cmp  "undefined var empty"          "echo \$TOTALLY_UNDEFINED_XYZ_VAR"
run_cmp  "dollar alone"                 "echo \$"
run_cmp  "dollar question (success)"    "true
echo \$?"
run_cmp  "dollar question (failure)"    "false
echo \$?"
run_cmp  "dollar digit no expand"       "echo \$1"
run_cmp  "var in single quotes"         "echo '\$HOME'"
run_cmp  "set and echo var"             "export MY_TEST_VAR=hello
echo \$MY_TEST_VAR"
run_cmp  "var in double quotes"         'export DQTEST=world
echo "hello $DQTEST"'
run_cmp  "unset var"                    "export UNSET_TEST=xyz
unset UNSET_TEST
echo \$UNSET_TEST"

# Compare output normalizing macOS /private symlinks
run_cmp_path() {
    local name="$1"
    local input="$2"
    TOTAL=$((TOTAL + 1))
    bash_out=$(printf '%s\n' "$input" | bash 2>/dev/null | sed 's|^/private||')
    mini_out=$(printf '%s\n' "$input" | "$SHELL_BIN" 2>/dev/null | sed 's|^/private||')
    if [ "$bash_out" = "$mini_out" ]; then
        echo -e "${GREEN}[PASS]${NC} $name"
        PASS=$((PASS + 1))
    else
        echo -e "${RED}[FAIL]${NC} $name"
        echo "  bash: $bash_out  mini: $mini_out"
        FAIL=$((FAIL + 1))
    fi
}

run_cmp  "pwd"                          "pwd"
run_cmp_path "cd /tmp and pwd"         "cd /tmp
pwd"
run_cmp_path "cd HOME"                  "cd
pwd"
run_grep "cd nonexistent"               "cd /nonexistent_xyz_12345"  "No such file"
run_cmp_path "cd with extra args"       "cd /tmp /var
pwd"

# =========================================================================== #
echo ""
echo -e "${YELLOW}--- 6. Export / Env / Unset ---${NC}"
# =========================================================================== #
run_grep "export no args has declare"   "export"                  "declare -x"
run_grep "export sets var in env"       "export T_MS=testval
env"                                                              "T_MS=testval"
run_grep "export invalid name"          "export 1BADNAME=x"       "not a valid identifier"
run_grep "env has PATH"                 "env"                     "PATH="
run_grep "env has HOME"                 "env"                     "HOME="
# Check unset removes variable — it should NOT appear in env output
TOTAL=$((TOTAL + 1))
unset_out=$(printf 'export GONE_VAR=yes\nunset GONE_VAR\nenv\n' | "$SHELL_BIN" 2>/dev/null)
if echo "$unset_out" | grep -q '^GONE_VAR'; then
    echo -e "${RED}[FAIL]${NC} unset removes from env (GONE_VAR still present!)"
    FAIL=$((FAIL + 1))
else
    echo -e "${GREEN}[PASS]${NC} unset removes from env"
    PASS=$((PASS + 1))
fi

# =========================================================================== #
echo ""
echo -e "${YELLOW}--- 7. Redirections ---${NC}"
# =========================================================================== #
run_cmp  "redir output (>)"            "echo hello > /tmp/ms_h_out.txt
cat /tmp/ms_h_out.txt
rm /tmp/ms_h_out.txt"
run_cmp  "redir append (>>)"           "echo line1 > /tmp/ms_h_app.txt
echo line2 >> /tmp/ms_h_app.txt
cat /tmp/ms_h_app.txt
rm /tmp/ms_h_app.txt"
run_cmp  "redir input (<)"            "echo hello > /tmp/ms_h_in.txt
cat < /tmp/ms_h_in.txt
rm /tmp/ms_h_in.txt"
run_grep "redir to bad path"           "echo hi > /no_such/dir/file.txt"  "No such file"
run_grep "redir input missing"         "cat < /no_such_file_xyz"          "No such file"
no_crash "redir output creates file"   "echo hi > /tmp/ms_redir_crash_test.txt
rm -f /tmp/ms_redir_crash_test.txt"

# =========================================================================== #
echo ""
echo -e "${YELLOW}--- 8. Pipelines ---${NC}"
# =========================================================================== #
run_cmp  "simple pipe"                  "echo hello | cat"
run_cmp  "two pipes"                    "echo hello | cat | cat"
run_cmp  "five pipes"                   "echo hello | cat | cat | cat | cat | cat"
run_cmp  "pipe with grep"               "printf 'foo\nbar\nbaz\n' | grep bar"
run_cmp  "pipe with wc -l"              "printf 'a\nb\nc\n' | wc -l"
no_crash "long pipeline (20 cats)"      "$(python3 -c "print('echo x | ' + ' | '.join(['cat']*20))")"
no_crash "pipe syntax error"            "| cat"
no_crash "pipe no right side"           "echo hi |"

# Heredoc tests: use temp files to feed multi-line input correctly
heredoc_cmp() {
    local name="$1"
    local script="$2"
    TOTAL=$((TOTAL + 1))
    local tmpf
    tmpf=$(mktemp /tmp/ms_hd_XXXXXX.sh)
    printf '%s\n' "$script" > "$tmpf"
    bash_out=$(bash < "$tmpf" 2>/dev/null)
    mini_out=$("$SHELL_BIN" < "$tmpf" 2>/dev/null)
    rm -f "$tmpf"
    if [ "$bash_out" = "$mini_out" ]; then
        echo -e "${GREEN}[PASS]${NC} $name"
        PASS=$((PASS + 1))
    else
        echo -e "${RED}[FAIL]${NC} $name"
        echo "  bash: $(echo "$bash_out" | head -2)  mini: $(echo "$mini_out" | head -2)"
        FAIL=$((FAIL + 1))
    fi
}

heredoc_cmp "heredoc basic"              'cat << EOF
hello
EOF'
heredoc_cmp "heredoc expands vars"       'export HD_VAR=world
cat << EOF
hello $HD_VAR
EOF'
heredoc_cmp "heredoc no expand (quoted)" "cat << 'EOF'
hello \$HOME
EOF"
tmpf=$(mktemp /tmp/ms_hd_empty_XXXXXX.sh)
printf 'cat << DELIM\nDELIM\n' > "$tmpf"
no_crash "heredoc empty body"           "$(cat "$tmpf")"
rm -f "$tmpf"
no_crash "heredoc ctrl-d (EOF)"         "$(printf 'cat << NEVERENDING')"

# =========================================================================== #
echo ""
echo -e "${YELLOW}--- 10. Exit Builtin ---${NC}"
# =========================================================================== #
run_exit "exit 0"                       "exit 0"     0
run_exit "exit 42"                      "exit 42"    42
run_exit "exit 255"                     "exit 255"   255
run_exit "exit 256 wraps"               "exit 256"   0
run_exit "exit no args (last_exit)"     "exit"       0
run_grep "exit non-numeric"             "exit abc"   "numeric argument required"
no_crash "exit too many args no exit"   "exit 1 2"

# =========================================================================== #
echo ""
echo -e "${YELLOW}--- 11. Path Resolution ---${NC}"
# =========================================================================== #
run_cmp  "absolute path"               "/bin/echo hello"
run_grep "nonexistent cmd"             "nonexistent_cmd_xyz_ms123"  "command not found"
run_exit "nonexistent cmd exit 127"    "nonexistent_cmd_xyz_ms123"  127
run_exit "directory as cmd exit 126"   "/tmp"                        126

# =========================================================================== #
echo ""
echo -e "${YELLOW}--- 12. Mixed Stress Tests ---${NC}"
# =========================================================================== #
no_crash "pipe redir combo"            "echo hello > /tmp/ms_combo.txt
cat < /tmp/ms_combo.txt | cat
rm /tmp/ms_combo.txt"
no_crash "many pipelines in sequence"  "$(for i in $(seq 1 10); do echo "echo x | cat"; done | tr '\n' '\n')"
no_crash "export then pipe"            "export TEST_MS=pipe
echo \$TEST_MS | cat"
no_crash "deeply nested quotes"        'echo "hello '"'"'world'"'"'"'

# =========================================================================== #
echo ""
echo -e "${YELLOW}--- 13. Parsing & Syntax (mandatory-style) ---${NC}"
# =========================================================================== #
no_crash "redir in then out"           "echo x > /tmp/ms_p1.txt
cat < /tmp/ms_p1.txt
rm -f /tmp/ms_p1.txt"
no_crash "multiple output redirs"      "echo hi > /tmp/ms_a.txt > /tmp/ms_b.txt
cat /tmp/ms_b.txt
rm -f /tmp/ms_a.txt /tmp/ms_b.txt"
run_cmp  "pipe then redir out"        "echo hello | cat > /tmp/ms_po.txt
cat /tmp/ms_po.txt
rm -f /tmp/ms_po.txt"
no_crash "empty command (only spaces)" "    "
no_crash "only redir token"            ">"
no_crash "redir then pipe"             "echo a > /tmp/ms_rp.txt | cat
rm -f /tmp/ms_rp.txt"
run_grep "syntax pipe first"           "| echo hi"  "syntax"
run_grep "syntax pipe last"            "echo hi |"  "syntax"
run_grep "syntax redir no file"        "echo hi >"  "syntax"

# =========================================================================== #
echo ""
echo -e "${YELLOW}--- 14. Builtin Exit Codes (bash-aligned) ---${NC}"
# =========================================================================== #
run_exit "exit negative"               "exit -1"    255
run_exit "exit 257 wraps to 1"        "exit 257"   1
run_grep "exit non-numeric stderr"     "exit abc"   "numeric argument required"
run_exit "exit too many args exit 1"  "exit 1 2 3" 1
run_exit "cd bad dir exit 1"          "cd /nonexistent_xyz_12345" 1
run_exit "export bad name exit 1"     "export 1BAD=x" 1

# =========================================================================== #
echo ""
echo -e "${YELLOW}--- 15. Variables & Expansion (mandatory) ---${NC}"
# =========================================================================== #
run_cmp  "empty var"                   "echo a\$EMPTY_VAR b"
run_cmp  "var with underscore"        "export A_B=1
echo \$A_B"
run_cmp  "question mark exit"         "false
echo \$?"
run_cmp  "dollar in single quote"      "echo '\$USER'"
run_grep "invalid export"              "export A-B=x" "not a valid identifier"
no_crash "expansion at end"            "echo \$"

# =========================================================================== #
echo ""
echo -e "${YELLOW}--- 16. Redirections (mandatory edge cases) ---${NC}"
# =========================================================================== #
run_cmp  "append then read"            "echo one > /tmp/ms_ap.txt
echo two >> /tmp/ms_ap.txt
cat < /tmp/ms_ap.txt
rm -f /tmp/ms_ap.txt"
no_crash "input redir missing file"    "cat < /tmp/does_not_exist_xyz_123"
run_grep "input missing err"           "cat < /tmp/does_not_exist_xyz_123" "No such file"
no_crash "output to dir (fail)"        "echo x > /tmp 2>/dev/null"
no_crash "heredoc then command"        "cat << END
hi
END
echo done"

# =========================================================================== #
echo ""
echo -e "${YELLOW}--- 17. Pipelines (mandatory) ---${NC}"
# =========================================================================== #
run_cmp  "pipe builtin echo"           "echo foo | cat"
run_cmp  "pipe with spaces"            "echo  a  b  c  | cat"
run_exit "pipe last cmd fails"          "true | false" 1
run_exit "pipe last cmd success"       "false | true" 0
no_crash "pipe with redir"             "echo x | cat > /tmp/ms_pwr.txt
cat /tmp/ms_pwr.txt
rm -f /tmp/ms_pwr.txt"

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
    echo -e "${GREEN}✅ All hardening tests passed!${NC}"
    exit 0
else
    PCTG_OK=$((PASS * 100 / TOTAL))
    echo -e "${YELLOW}⚠️  $FAIL test(s) failed ($PCTG_OK% pass rate).${NC}"
    exit 1
fi
