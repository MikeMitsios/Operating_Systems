teliko:main.o functions.o
	gcc main.o functions.o -o teliko 

main.o:main.c structs.h funct.h
	gcc -c main.c

functions.o:functions.c structs.h funct.h
	gcc -c functions.c

clean:
	rm teliko main.o functions.o			