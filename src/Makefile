CFLAGS = -c -Wall -ansi -pedantic

list: runsymtab.o symtablelist.o
	gcc runsymtab.o symtablelist.o -o list

runsymtab.o: runsymtab.c symtable.h
	gcc $(CFLAGS) runsymtab.c

symtablelist.o: symtablelist.c symtable.h
	gcc $(CFLAGS) symtablelist.c

clean:
	rm -f *.o list