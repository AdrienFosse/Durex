CC=gcc

NAME_SERVER = Durex
NAME_CLIENT = Ben_AFK

SRCS_CLIENT =	./client/sources/main.c \
				./client/sources/client.c \
				./common/sources/socket.c \
				./common/sources/rsa.c \
				./common/sources/aes.c
				
SRC_SERVER = 	./server/sources/main.c \
				./server/sources/daemon.c \
				./server/sources/shell.c \
				./common/sources/socket.c \
				./common/sources/rsa.c \
				./common/sources/aes.c 

OBJ_CLIENT = $(SRCS_CLIENT:.c=.o)
OBJ_SERVER = $(SRC_SERVER:.c=.o)

all: $(NAME_CLIENT) $(NAME_SERVER)

client: $(NAME_CLIENT)

server: $(NAME_SERVER)

$(NAME_CLIENT): $(OBJ_CLIENT)
	@$(CC) $(CFLAGS) $(OBJ_CLIENT) -o $(NAME_CLIENT) -I ./daemon/client/headers/ -I ./daemon/common/headers/ -L /usr/local/ssl/lib -lssl -lcrypto ||:

$(NAME_SERVER): $(OBJ_SERVER)
	@$(CC) $(CFLAGS) $(OBJ_SERVER) -o $(NAME_SERVER) -I ./daemon/server/headers/ -I ./daemon/common/headers/ -L /usr/local/ssl/lib -lssl -lcrypto -lpthread ||:

clean:
	@rm -f $(OBJ_CLIENT) ||:
	@rm -f $(OBJ_SERVER) ||:

fclean: clean
	@rm -f $(NAME_CLIENT) ||:
	@rm -f $(NAME_SERVER) ||:

re: fclean all

.PHONY: clean fclean re
