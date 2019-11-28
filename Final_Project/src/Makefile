#_*_ Makefile _*_

CC=gcc

CFLAGS = -I../inc/ -pthread -lrt -Werror

vpath %.h ../inc/

OBJ = main.o FG_Locking_BST.o

concurrent_tree: $(OBJ)
	$(CC) -o concurrent_tree $(OBJ) -lpthread -lrt

main.o: main.c main.h
	$(CC) -c -g main.c $(CFLAGS)

FG_Locking_BST.o: FG_Locking_BST.c FG_Locking_BST.h
	$(CC) -c -g FG_Locking_BST.c $(CFLAGS)

clean:
	-rm -f *.o concurrent_tree
