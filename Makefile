#Autor: Alex Leidwein
CC = /usr/bin/gcc
SRC = mqdivser.c mqdivcli.c
BIN = $(SRC:%.c=%.out)

all: mqdivser mqdivcli
		
mqdivser: mqdivser.c
	gcc mqdivser.c -o mqdivser.out

mqdivcli: mqdivcli.c
	gcc mqdivcli.c -o mqdivcli.out

.PHONY: clean
clean:
	rm -rf $(BIN)