CFLAGS := -std=c99 -Wall -Werror -pedantic -ggdb

eight-queens: main.c
	$(CC) $(CFLAGS) -o $@ $^
