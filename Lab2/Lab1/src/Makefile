# _*_ Makefile _*_

CC=g++

CFLAGS = -Wall -pthread -g

OBJ = mysort.o print_on_console.o quick_sort.o print_in_file.o bucket_sort.o sense_reversal_barrier.o locks.o

mysort:$(OBJ)
	$(CC) $(CFLAGS) -o mysort $(OBJ)

mysort.o:mysort.c
	$(CC) $(CFLAGS) -c mysort.c

print_on_console.o:print_on_console.c
	$(CC) $(CFLAGS) -c print_on_console.c

quick_sort.o:quick_sort.c
	$(CC) $(CFLAGS) -c quick_sort.c

print_in_file.o:print_in_file.c
	$(CC) $(CFLAGS) -c print_in_file.c

bucket_sort.o:bucket_sort.c
	$(CC) $(CFLAGS) -c bucket_sort.c

sense_reversal_barrier.o:sense_reversal_barrier.c
	$(CC) $(CFLAGS) -c sense_reversal_barrier.c

locks.o:locks.c
	$(CC) $(CFLAGS) -c locks.c

clean:
	rm *.o mysort output.txt
