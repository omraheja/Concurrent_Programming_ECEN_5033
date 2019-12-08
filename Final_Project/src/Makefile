#_*_ Makefile _*_

CC=g++

CFLAGS = -I../inc/ -pthread -lrt -Werror

vpath %.h ../inc/

OBJ = main.o FG_Locking_BST.o

all:concurrent_tree unit_test
	@echo "\n*************************************************************************************"
	@echo "*\t\t\tHOW TO RUN THE CODE?\t\t\t                    *"
	@echo "*\t\t\tType './concurrent_tree --help'\t\t\t            *"
	@echo  "*************************************************************************************"


concurrent_tree: $(OBJ)
	$(CC) -o concurrent_tree $(OBJ) -lpthread -lrt

unit_test: FG_Locking_BST.o unit_test.o
	$(CC) -o unit_test FG_Locking_BST.o unit_test.o -lpthread -lrt -lcunit

main.o: main.cpp main.h
	$(CC) -c -g main.cpp $(CFLAGS)

FG_Locking_BST.o: FG_Locking_BST.c FG_Locking_BST.h
	$(CC) -c -g FG_Locking_BST.c $(CFLAGS)


unit_test.o: unit_test.c main.h FG_Locking_BST.h
	$(CC) -c -g unit_test.c $(CFLAGS) -lcunit

clean:
	-rm -f *.o *.xml concurrent_tree unit_test ans.txt
