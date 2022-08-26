CC = gcc 
#CFLAGS = -Wall -Wextra -Werror
SERVER_NAME = server
SERVER_SRC = server.c
CLIENT_NAME = client
CLIENT_SRC = sender.c client.c collector.c read_file.c
UTILS_SRC = utils.c struct.c
DB_FLAG = -I/usr/include/mysql -L/usr/lib/mysql -lmysqlclient

SERVER_OBJECTS = $(SERVER_SRC:%.c=%.o)
CLIENT_OBJECTS = $(CLIENT_SRC:%.c=%.o)
UTILS_OBJECTS = $(UTILS_SRC:%.c=%.o)

all : $(SERVER_NAME) $(CLIENT_NAME)

$(SERVER_NAME) : $(UTILS_OBJECTS) $(SERVER_OBJECTS)
	$(CC) $(CFLAGS) $(UTILS_OBJECTS) $(SERVER_OBJECTS) -o $(SERVER_NAME)
	
$(CLIENT_NAME) : $(CLIENT_OBJECTS) $(UTILS_OBJECTS) 
	$(CC) $(CFLAGS) $(CLIENT_OBJECTS) $(UTILS_OBJECTS) -o $(CLIENT_NAME)

clean :
	rm -rf $(SERVER_OBJECTS) $(CLIENT_OBJECTS) $(UTILS_OBJECTS)

fclean :
	rm -rf $(SERVER_OBJECTS) $(CLIENT_OBJECTS) $(UTILS_OBJECTS) $(CLIENT_NAME) $(SERVER_NAME)

re : fclean all
