#all: mqdivser.c mqdivcli.c
#	gcc -o mqdivser mqdivcli mqdivser.c mqdivcli.c
	
mqdivser: mqdivser.c
	gcc -o mqdivser mqdivser.c

mqdivcli: mqdivcli.c
	gcc -o mqdivcli mqdivcli.c