OBJS = HT.o Main.o SHT.o Functions.o

SOURCE = HT.c Main.c SHT.c Functions.c 

HEADER = HT.h SHT.h Types.h

OUT = Main

CC = gcc

FLAGS = -g -c

all: Main

Main: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) BF_64.a -no-pie

Main.o: Main.c
	$(CC) $(FLAGS) Main.c

HT.o: HT.c
	$(CC) $(FLAGS) HT.c

SHT.o: SHT.c
	$(CC) $(FLAGS) SHT.c

Functions.o: Functions.c
	$(CC) $(FLAGS) Functions.c

run:
	./$(OUT) records1K.txt

clean:
	rm -f $(OBJS) $(OUT)
