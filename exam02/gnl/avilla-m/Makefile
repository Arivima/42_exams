


NAME = 		gnl

SRCS =      get_next_line.c\
			main.c\


OBJS = 		$(SRCS:.c=.o)

RM = 		rm -f
CC = 		gcc
CFLAGS = 	-Wall -Wextra -Werror -I . -D BUFFER_SIZE=20

%.o :		%.c 		
			$(CC) $(CFLAGS) -c $< -o $@

$(NAME): 	$(OBJS)
			$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

all: 		$(NAME)

clean :
			$(RM) $(OBJS) *.o

fclean : 	clean
			$(RM) $(NAME)
xclean :	clean
			$(RM) *.out


re : 		fclean all

.PHONY: 	all clean fclean re