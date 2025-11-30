# **************************************************************************** #
#                                  PROJECT                                     #
# **************************************************************************** #

NAME        = minishell

CC          = cc
CFLAGS      = -Wall -Wextra -Werror -I$(INCLUDES) -Ilibft

SRC_DIR     = src
OBJ_DIR     = obj
INCLUDES 	= includes
LIBFT_DIR	= libft
RLFLAGS		= -lreadline -lncurses

# --------------------- MANUALLY LIST YOUR SOURCE FILES ---------------------- #
# main
SRCS        = $(SRC_DIR)/main.c \
				$(SRC_DIR)/init.c \
				$(SRC_DIR)/utils.c \
				$(SRC_DIR)/free.c

# signals
#SRCS        += $(SRC_DIR)/signals/signal_handlers.c \
               $(SRC_DIR)/signals/signal_init.c

# parser
SRCS        += $(SRC_DIR)/parser/parser.c \
               $(SRC_DIR)/parser/tokens.c \
#               $(SRC_DIR)/parser/lexer.c

# executor
#SRCS        += $(SRC_DIR)/executor/executor.c \
               $(SRC_DIR)/executor/commands.c

# builtins
# builtins - uncomment/add the specific builtins as you implement them
SRCS        += $(SRC_DIR)/builtins/cd.c \
			   $(SRC_DIR)/builtins/pwd.c \
			   $(SRC_DIR)/builtins/env.c \
			   $(SRC_DIR)/builtins/echo.c \
			   $(SRC_DIR)/builtins/export.c \
			   $(SRC_DIR)/builtins/unset.c \
			   $(SRC_DIR)/builtins/exit.c

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

debug: CFLAGS += -g
debug: re

.PHONY: all clean fclean re debug
