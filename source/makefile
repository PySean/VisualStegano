VPATH = parseimg:filter:normal:spreadspec:errorcc
CC=gcc
CFLAGS=-Wall -c

objects = parseimg.o main.o filter.o normal.o normsinv.o spread_spectrum.o errorcc.o

hideit: $(objects)
	$(CC) -o hideit $(objects) -lm
main.o: main.c 
	$(CC) $(CFLAGS) main.c
parseimg.o: parseimg.c parseimg.h
	$(CC) $(CFLAGS) ./parseimg/parseimg.c
filter.o: filter.c filter.h
	$(CC) $(CFLAGS) ./filter/filter.c
normal.o: normal.c normal.h
	$(CC) $(CFLAGS) ./normal/normal.c
normsinv.o: normsinv.c normsinv.h
	$(CC) $(CFLAGS) ./normal/normsinv.c
spread_spectrum.o: spread_spectrum.c spread_spectrum.h
	$(CC) $(CFLAGS) ./spreadspec/spread_spectrum.c
errorcc.o: errorcc.c errorcc.h
	$(CC) $(CFLAGS) ./errorcc/errorcc.c
clean:
	rm hideit $(objects)
