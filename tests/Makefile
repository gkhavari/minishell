# Phase 1 Tests Makefile

CC = cc
CFLAGS = -Wall -Wextra -Werror

# Paths (relative to tests directory)
INCLUDES = -I../includes -I../libft
LIBFT = ../libft/libft.a

# Builtin sources
BUILTIN_DIR = ../src/builtins
BUILTIN_SRCS = $(BUILTIN_DIR)/echo.c \
               $(BUILTIN_DIR)/cd.c \
               $(BUILTIN_DIR)/pwd.c \
               $(BUILTIN_DIR)/env.c \
               $(BUILTIN_DIR)/export.c \
               $(BUILTIN_DIR)/export_utils.c \
               $(BUILTIN_DIR)/unset.c \
               $(BUILTIN_DIR)/exit.c

# Utils needed
UTILS_SRCS = ../src/utils.c ../src/init.c

# Test target
TEST_BUILTINS = test_builtins

all: $(TEST_BUILTINS)

$(TEST_BUILTINS): test_builtins.c $(BUILTIN_SRCS) $(UTILS_SRCS) $(LIBFT)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -lreadline -o $@
	@echo "✅ Built $(TEST_BUILTINS)"

$(LIBFT):
	@make -C ../libft

run: $(TEST_BUILTINS)
	@echo ""
	@./$(TEST_BUILTINS)

# Integration test (shell script)
test_integration:
	@chmod +x test_phase1.sh
	@./test_phase1.sh

clean:
	rm -f $(TEST_BUILTINS)

.PHONY: all run test_integration clean
