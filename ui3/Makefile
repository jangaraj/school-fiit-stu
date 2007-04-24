#compile:
#	gcc -Wall -g -c genotyp.c -L/home/vlk/bin/lib
#	gcc -Wall -g -o xui3 ui3.c genotyp.o -L/home/vlk/bin/lib -lmatheval

xui3: ui3.c genotyp.o const.h
	gcc -Wall -g -o $@ genotyp.o -L/home/vlk/bin/lib -lmatheval ui3.c

genotyp.o: genotyp.c genotyp.h const.h
	gcc -Wall -g -c -std=c99 -L/home/vlk/bin/lib genotyp.c 

