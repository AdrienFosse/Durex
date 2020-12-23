CC=gcc 

NAME_DUREX = Durex

SRC_DUREX = 	./durex/sources/main.c

OBJ_DUREX = $(SRC_DUREX:.c=.o)

all: $(NAME_DUREX)

durex: $(NAME_DUREX)

$(NAME_DUREX): $(OBJ_DUREX)
	$(CC) $(CFLAGS) $(OBJ_DUREX) -o $(NAME_DUREX) -I ./durex/headers/
	@echo "\033[36m◉ \033[33mmake durex is done\033[0m"

clean:
	rm -f $(OBJ_DUREX)
	@echo "\033[36m◉ \033[33mclean done\033[0m"

fclean: clean
	rm -f $(NAME_DUREX)
	@echo "\033[36m◉ \033[33mfclean done\033[0m"

re: fclean all

.PHONY: clean fclean re
