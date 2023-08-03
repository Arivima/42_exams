# Makefile

NAME = 		gnl

SRCS =      get_next_line.c\
			main.c\

all: 		
			gcc -Wall -Wextra -Werror -D BUFFER_SIZE=20 $(SRCS) -o gnl

clean :
			rm -rf *.o

fclean : 	clean
			rm -rf $(NAME)

re : 		fclean all

.PHONY: 	all clean fclean re