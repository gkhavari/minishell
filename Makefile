# **************************************************************************** #
#                                  PROJECT                                     #
# **************************************************************************** #

NAME        = minishell

CC          = cc
CFLAGS      = -Wall -Wextra -Werror -Iincludes -Ilibft

SRC_DIR     = src
OBJ_DIR     = obj
LIBFT_DIR	= libft

# --------------------- MANUALLY LIST YOUR SOURCE FILES ---------------------- #
# main
SRCS        = $(SRC_DIR)/main.c

# signals
SRCS        += $(SRC_DIR)/signals/signal_handlers.c \
               $(SRC_DIR)/signals/signal_init.c

# parser
SRCS        += $(SRC_DIR)/parser/parser.c \
               $(SRC_DIR)/parser/lexer.c \
               $(SRC_DIR)/parser/tokens.c

# executor
SRCS        += $(SRC_DIR)/executor/executor.c \
               $(SRC_DIR)/executor/commands.c

# builtins
SRCS        += $(SRC_DIR)/builtins/echo.c \
               $(SRC_DIR)/builtins/cd.c \
               $(SRC_DIR)/builtins/pwd.c

# --------------------------------------------------------------------------- #

OBJS        = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
LIBFT       = $(LIBFT_DIR)/libft.a

# **************************************************************************** #
#                                   RULES                                      #
# **************************************************************************** #

all: $(LIBFT) $(NAME)

# Build minishell
$(NAME): $(OBJ_DIR) $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) $(LIBFT) $(RLFLAGS) -o $(NAME)
	@echo "🔥 minishell built"

# Build libft
$(LIBFT):
	@$(MAKE) -C $(LIBFT_DIR)
	@echo "📚 libft built"

# Create obj/ and subdirectories manually
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(OBJ_DIR)/signals
	@mkdir -p $(OBJ_DIR)/parser
	@mkdir -p $(OBJ_DIR)/executor
	@mkdir -p $(OBJ_DIR)/builtins

# Compile .c → .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiling $<"

# Clean object files
clean:
	@rm -rf $(OBJ_DIR)
	@$(MAKE) -C $(LIBFT_DIR) clean
	@echo "🧹 Object files removed"

# Clean everything
fclean: clean
	@rm -f $(NAME)
	@$(MAKE) -C $(LIBFT_DIR) fclean
	@echo "🧽 Executable removed"

re: fclean all

.PHONY: all clean fclean re
