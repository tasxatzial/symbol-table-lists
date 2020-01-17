CFLAGS = -c -Wall -ansi -pedantic

list: testsymtab.o symtablelist.o
	gcc testsymtab.o symtablelist.o -o list

testsymtab.o: testsymtab.c symtable.h
	gcc $(CFLAGS) testsymtab.c

symtablelist.o: symtablelist.c symtable.h
	gcc $(CFLAGS) symtablelist.c

clean:
	rm -f *.o list