NAME = libft.a

SRCS = ft_isdigit.c ft_memset.c ft_strjoin.c ft_strtrim.c ft_isprint.c\
ft_isspace.c\
ft_putchar_fd.c ft_strlcat.c ft_substr.c ft_atoi.c ft_itoa.c ft_putendl_fd.c\
ft_strlcpy.c ft_tolower.c ft_bzero.c ft_putnbr_fd.c ft_strlen.c\
ft_toupper.c ft_calloc.c ft_memchr.c ft_putstr_fd.c ft_strmapi.c ft_isalnum.c\
ft_memcmp.c ft_split.c ft_strncmp.c ft_strcmp.c ft_isalpha.c ft_memcpy.c ft_strchr.c\
ft_strnstr.c ft_isascii.c ft_memmove.c ft_strdup.c ft_strrchr.c ft_striteri.c\
ft_strndup.c get_next_line.c get_next_line_utils.c\
ft_printf.c ft_printf_chr.c ft_printf_str.c ft_printf_ptr.c ft_printf_nbr.c\
ft_printf_hex.c ft_printf_unbr.c

BONUS = ft_lstnew_bonus.c ft_lstadd_front_bonus.c ft_lstlast_bonus.c\
ft_lstsize_bonus.c ft_lstadd_back_bonus.c ft_lstdelone_bonus.c\
ft_lstclear_bonus.c ft_lstiter_bonus.c ft_lstmap_bonus.c

OBJS = ${SRCS:.c=.o}
OBJSBONUS = ${BONUS:.c=.o}

SMOKE	= ft_printf_smoke

CC      = cc
RM      = rm -f
CFLAGS  = -Wall -Wextra -Werror

# Compile .c → .o
.c.o:
	@${CC} ${CFLAGS} -c $< -o ${<:.c=.o}

# Build library
$(NAME): ${OBJS}
	@ar rcs ${NAME} ${OBJS}

bonus: ${OBJS} ${OBJSBONUS}
	@ar rcs ${NAME} ${OBJS} ${OBJSBONUS}

all: $(NAME)

clean:
	@${RM} ${OBJS} ${OBJSBONUS} $(SMOKE)

fclean: clean
	@${RM} ${NAME}

re: fclean all

# Local leak check for ft_printf (not linked into minishell build)
$(SMOKE): all
	@$(CC) $(CFLAGS) -I. ../scripts/ft_printf_smoke.c $(NAME) -o $(SMOKE)
	@echo "Built $(SMOKE) (make -C libft smoke_printf)"

smoke_printf: $(SMOKE)

.PHONY: all clean fclean re bonus smoke_printf