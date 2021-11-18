CC=gcc

default: main.o x_pbit.o
	$(CC) -o xpbit main.o x_pbit.o
all: main.o x_pbit.o
	$(CC) -o xpbit main.o x_pbit.o
clean:
	rm -f ./*.o
	rm -f ./xpbit