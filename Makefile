# **************************************************************************** #
#                                  PROJECT                                     #
# **************************************************************************** #

NAME        = minishell

CC          = cc
RL_DIR		= /opt/homebrew/opt/readline
CFLAGS      = -Wall -Wextra -Werror -I$(INCLUDES) -Ilibft -I$(RL_DIR)/include

SRC_DIR     = src
OBJ_DIR     = obj
INCLUDES 	= includes
LIBFT_DIR	= libft
RLFLAGS		= -L$(RL_DIR)/lib -lreadline -lncurses

# --------------------- MANUALLY LIST YOUR SOURCE FILES ---------------------- #
# main
SRCS		=	$(SRC_DIR)/main.c \
				$(SRC_DIR)/init.c \
				$(SRC_DIR)/utils.c \
				$(SRC_DIR)/free.c \
				$(SRC_DIR)/free_utils.c
				$(SRC_DIR)/free_runtime.c \
				$(SRC_DIR)/free_shell.c 

# signals
SRCS		+=	$(SRC_DIR)/signals/signal_handler.c \
				$(SRC_DIR)/signals/signal_utils.c

# tokenizer
SRCS		+=	$(SRC_DIR)/tokenizer/tokenizer.c \
				$(SRC_DIR)/tokenizer/tokenizer_utils.c \
				$(SRC_DIR)/tokenizer/tokenizer_utils2.c \
				$(SRC_DIR)/tokenizer/tokenizer_ops.c \
				$(SRC_DIR)/tokenizer/expansion.c \
				$(SRC_DIR)/tokenizer/expansion_utils.c \
				$(SRC_DIR)/tokenizer/tokenizer_handlers.c \
				$(SRC_DIR)/tokenizer/tokenizer_quotes.c \
				$(SRC_DIR)/tokenizer/continuation.c

# parser
SRCS		+=	$(SRC_DIR)/parser/parser.c \
				$(SRC_DIR)/parser/parser_syntax_check.c \
				$(SRC_DIR)/parser/add_token_to_cmd.c \
				$(SRC_DIR)/parser/argv_build.c \
				$(SRC_DIR)/parser/heredoc.c \
				$(SRC_DIR)/parser/heredoc_utils.c
				$(SRC_DIR)/parser/heredoc.c

# utils
SRCS		+=	$(SRC_DIR)/utils/simple_split.c \
				$(SRC_DIR)/utils/arrdup.c

# executor
SRCS        += $(SRC_DIR)/executor/executor.c \
			   $(SRC_DIR)/executor/executor_utils.c \
			   $(SRC_DIR)/executor/executor_external.c \
			   $(SRC_DIR)/executor/executor_pipeline.c \
			   $(SRC_DIR)/executor/executor_child.c

# builtins
SRCS        += $(SRC_DIR)/builtins/cd.c \
			   $(SRC_DIR)/builtins/pwd.c \
			   $(SRC_DIR)/builtins/env.c \
			   $(SRC_DIR)/builtins/echo.c \
			   $(SRC_DIR)/builtins/export.c \
			   $(SRC_DIR)/builtins/export_utils.c \
			   $(SRC_DIR)/builtins/unset.c \
			   $(SRC_DIR)/builtins/exit.c \
			   $(SRC_DIR)/builtins/export_print.c \
			   $(SRC_DIR)/builtins/builtin_dispatcher.c

# --------------------------------------------------------------------------- #

OBJS        = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
LIBFT       = $(LIBFT_DIR)/libft.a

# **************************************************************************** #
#                                   RULES                                      #
# **************************************************************************** #

all: $(NAME)

# Build minishell
$(NAME): $(LIBFT) $(OBJ_DIR) $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) $(LIBFT) $(RLFLAGS) -o $(NAME)
	@echo "minishell built"

# Build libft
$(LIBFT):
	@$(MAKE) -C $(LIBFT_DIR)
	@echo "libft built"

# Create obj/ and subdirectories manually
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(OBJ_DIR)/signals
	@mkdir -p $(OBJ_DIR)/tokenizer
	@mkdir -p $(OBJ_DIR)/parser
	@mkdir -p $(OBJ_DIR)/executor
	@mkdir -p $(OBJ_DIR)/builtins
	@mkdir -p $(OBJ_DIR)/utils

# Compile .c -> .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiling $<"

# Clean object files
clean:
	@rm -rf $(OBJ_DIR)
	@$(MAKE) -C $(LIBFT_DIR) clean
	@echo "Object files removed"

# Clean everything
fclean: clean
	@rm -f $(NAME)
	@$(MAKE) -C $(LIBFT_DIR) fclean
	@echo "Executable removed"

re: fclean all

debug: CFLAGS += -g
debug: re

.PHONY: all clean fclean re debug
