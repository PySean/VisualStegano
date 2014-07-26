VPATH = parseimg:filter

objects = parseimg.o main.o filter.o

hideit: $(objects)
	gcc -o hideit $(objects)
main.o: main.c 
	gcc -c main.c
parseimg.o: parseimg.c parseimg.h
	gcc -c ./parseimg/parseimg.c
filter.o: filter.c filter.h
	gcc -c ./filter/filter.c
clean:
	rm hideit $(objects)
