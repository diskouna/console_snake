CC 		:= clang
CFLAGS 	:= -Wall -Wextra -Werror -std=c11

.PHONY : all clean

all				: console_snake
	./console_snake
console-snake	: console_snake.c
	@$(CC) $(CFLAGS) $^ -o $@
clean :
	rm -f console_snake