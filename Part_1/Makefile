OBJS = HP.o HP_Main.o Record.o
OBJS2 = HT_Main.o HT.o Record.o
SOURCE = HP.c HP_Main.c Record.c
SOURCE2 = HT_Main.c HT.c Record.c
HEADER = HP.h HT.h Record.h
OUT1 = HP_Main
OUT2 = HT_Main
CC = gcc
FLAGS = -g -c

all: HP_Main HT_Main

HP_Main: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT1) BF_64.a -no-pie

HT_Main: $(OBJS2)
	$(CC) -g $(OBJS2) -o $(OUT2) BF_64.a -no-pie

HP.o: HP.c
	$(CC) $(FLAGS) HP.c

HP_Main.o: HP_Main.c
	$(CC) $(FLAGS) HP_Main.c

HT_Main.o: HT_Main.c
	$(CC) $(FLAGS) HT_Main.c

HT.o: HT.c
	$(CC) $(FLAGS) HT.c

Record.o: Record.c
	$(CC) $(FLAGS) Record.c

run1:
	./$(OUT1) records1K.txt

run2:
	./$(OUT2) records1K.txt

clean:
	rm -f $(OBJS) $(OUT1) $(OBJS2) $(OUT2)

