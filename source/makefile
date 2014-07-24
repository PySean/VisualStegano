VPATH = parseimg

objects = parseimg.o main.o

hideit: $(objects)
	gcc -o hideit $(objects)
main.o: main.c 
	gcc -c main.c
parseimg.o: parseimg.c parseimg.h
	gcc -c ./parseimg/parseimg.c
clean:
	rm hideit $(objects)
