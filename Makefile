CC 		:= clang
CFLAGS 	:= -Wall -Wextra -Werror -O2 -std=c11

.PHONY : all clean

all	: console_snake
	./console_snake
%	: %.c %.h
	$(CC) $(CFLAGS) $< -o $@
clean :
	rm -f console_snake