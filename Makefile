CFLAGS := -O3 -std=c99 -Wall -Werror -pedantic -ggdb

.PHONY: run build

build: eight-queens queenmoves

run: eight-queens queenmoves
	./eight-queens ./queenmoves ./sols

eight-queens: main.c defs.h
	$(CC) $(CFLAGS) -o $@ main.c 

queenmoves: gen 
	./gen queenmoves

gen: gen.c defs.h
	$(CC) $(CFLAGS) -o $@ gen.c
