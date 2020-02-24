build: functii.o main.o
	gcc -Wall functii.o main.o -o tema3 -lm

functii.o: functii.c
	gcc -Wall -c functii.c -lm

calcule.o: main.c
	gcc -Wall -c main.c -lm

clean:
	rm -rf *.o *.out tema3
