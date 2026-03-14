# **************************************************************************** #
#  Tests Makefile — reflects current project status                            #
#  Finished: phase1 (foundation & builtins), hardening (mandatory-style).     #
#  Run: make test   (runs both suites; both pass as of last run).             #
# **************************************************************************** #

CC       = cc
CFLAGS   = -Wall -Wextra -Werror
INCLUDES = -I../includes -I../libft
LIBFT    = ../libft/libft.a

# -----------------------------------------------------------------------------
#  Primary tests (finished, passing)
# -----------------------------------------------------------------------------
.PHONY: all test test_phase1 test_hardening clean help

all: test

# Run all integration tests (phase1 + hardening)
test: test_phase1 test_hardening
	@echo ""
	@echo "✅ All tests finished (phase1 + hardening)."

# Phase 1: foundation & builtins (24 tests) — run from repo root so ./minishell exists
test_phase1:
	@cd .. && chmod +x tests/test_phase1.sh 2>/dev/null || true
	@cd .. && bash tests/test_phase1.sh

# Hardening: mandatory-style edge cases (106 tests)
test_hardening:
	@cd .. && chmod +x tests/test_hardening.sh 2>/dev/null || true
	@cd .. && bash tests/test_hardening.sh

# -----------------------------------------------------------------------------
#  Optional: C unit test for builtins (needs get_env_value in test_builtins.c)
#  Uncomment and fix test_builtins.c (get_env -> get_env_value) to build.
# -----------------------------------------------------------------------------
# BUILTIN_DIR = ../src/builtins
# BUILTIN_SRCS = $(BUILTIN_DIR)/echo.c $(BUILTIN_DIR)/cd.c $(BUILTIN_DIR)/pwd.c \
#                $(BUILTIN_DIR)/env.c $(BUILTIN_DIR)/export.c $(BUILTIN_DIR)/export_utils.c \
#                $(BUILTIN_DIR)/unset.c $(BUILTIN_DIR)/exit.c
# UTILS_SRCS = ../src/utils.c ../src/init.c
# TEST_BUILTINS = test_builtins
# $(TEST_BUILTINS): test_builtins.c $(BUILTIN_SRCS) $(UTILS_SRCS) $(LIBFT)
# 	$(CC) $(CFLAGS) $(INCLUDES) $^ -lreadline -o $@
# run_builtins: $(TEST_BUILTINS)
# 	@./$(TEST_BUILTINS)

# -----------------------------------------------------------------------------
clean:
	@rm -f test_builtins 2>/dev/null || true
	@echo "Tests clean (scripts unchanged)."

help:
	@echo "Targets:"
	@echo "  make test          - Run phase1 + hardening (default)"
	@echo "  make test_phase1   - Run phase1 only (24 tests)"
	@echo "  make test_hardening - Run hardening only (106 tests)"
	@echo "  make clean         - Remove test_builtins binary if present"
	@echo "  make help          - This help"
