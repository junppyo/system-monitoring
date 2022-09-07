CC = gcc
NAME = client
CLIENT_INC = ./inc
CLIENT_SRC = client collector sender
UTIL_INC = ../util
UTIL_SRC = struct utils
SRC =  $(addsuffix .c, $(addprefix src/, $(CLIENT_SRC))) \
		$(addsuffix .c, $(addprefix ../util/, $(UTIL_SRC)))
OBJ = $(SRC:%.c=%.o)

PRELOAD_SRC = ./preload/preload.c
PRELOAD_NAME = ./preload/preload.so

LIB_SRC = ./lib/read_file.c
LIB_NAME = ./lib/readfile.so

all : $(LIB_NAME) $(NAME) $(PRELOAD_NAME)

$(NAME) : $(OBJ)
	$(CC) $(OBJ) -I $(CLIENT_INC) -I $(UTIL_INC) $(LIB_NAME) -o $(NAME)

$(PRELOAD_NAME) : $(PRELOAD_SRC)
	$(CC) -fPIC -shared $(PRELOAD_SRC) -I $(UTIL_INC) -o $(PRELOAD_NAME) -ldl

$(LIB_NAME) : $(LIB_SRC)
	$(CC) -fPIC -shared $(LIB_SRC) -I $(UTIL_INC) -o $(LIB_NAME) -ldl

%.o: %.c
	$(CC) -I $(CLIENT_INC) -I $(UTIL_INC) -c $< -o $@


clean :
	rm -f $(OBJ)

fclean :
	rm -f $(OBJ) $(NAME)

re : fclean all

.PHONY: all clean fclean re
