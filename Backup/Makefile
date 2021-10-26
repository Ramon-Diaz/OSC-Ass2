prodcon: prodcon.o tands.o
	@gcc -o prodcon prodcon.o tands.o -pthread

prodcon.o: prodcon.c
	@gcc -c prodcon.c

tands.o: tands.c
	@gcc -c tands.c

clean:
	@rm *.o prodcon