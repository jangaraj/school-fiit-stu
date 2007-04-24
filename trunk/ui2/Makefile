Wall: compile
ex: execute
iactive: interactive

compile:
	gcc -g -Wall -c split_stdin.c
	gcc -g -Wall -c hash_table.c
	gcc -g -Wall -c front.c
	gcc -g -Wall -o xui ui.c split_stdin.o hash_table.o front.o
execute:
	./xui
interactive:
	./xui -i
