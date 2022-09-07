CC = gcc
FLAGS = -lmysqlclient
NAME = server
SERVER_INC = ./inc
SERVER_SRC = receiver saver server
UTIL_INC = ../util
UTIL_SRC = struct utils
SRC =  $(addsuffix .c, $(addprefix src/, $(SERVER_SRC))) \
		$(addsuffix .c, $(addprefix ../util/, $(UTIL_SRC)))

OBJ = $(SRC:%.c=%.o)

all : $(NAME)

$(NAME) : $(OBJ)
	$(CC) $(OBJ) -I $(SERVER_INC) -I $(UTIL_INC) -o $(NAME) $(FLAGS)

%.o: %.c
	$(CC) -I $(SERVER_INC) -I $(UTIL_INC) -c $< -o $@


clean :
	rm -f $(OBJ)

fclean :
	rm -f $(OBJ) $(NAME)

re : fclean all

.PHONY: all clean fclean re
