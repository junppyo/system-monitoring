CC = gcc
NAME = client
CLIENT_INC = ./inc
CLIENT_SRC = client collector read_file sender
UTIL_INC = ../util
UTIL_SRC = struct utils
SRC =  $(addsuffix .c, $(addprefix src/, $(CLIENT_SRC))) \
		$(addsuffix .c, $(addprefix ../util/, $(UTIL_SRC)))
OBJ = $(SRC:%.c=%.o)

PRELOAD_SRC = ./preload/preload.c
PRELOAD_NAME = ./preload/preload.so

all : $(NAME) $(PRELOAD_NAME)

$(NAME) : $(OBJ)
	$(CC) $(OBJ) -I $(CLIENT_INC) -I $(UTIL_INC) -o $(NAME)

$(PRELOAD_NAME) : $(PRELOAD_SRC)
	$(CC) -fPIC -shared $(PRELOAD_SRC) -I $(UTIL_INC) -o $(PRELOAD_NAME) -ldl

%.o: %.c
	$(CC) -I $(CLIENT_INC) -I $(UTIL_INC) -c $< -o $@


clean :
	rm -f $(OBJ)

fclean :
	rm -f $(OBJ) $(NAME)

re : fclean all

.PHONY: all clean fclean re
