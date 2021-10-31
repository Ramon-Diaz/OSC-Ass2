prodcon: prodcon.o tands.o timer_.o
	@gcc -pthread -o prodcon prodcon.o tands.o timer_.o

prodcon.o: prodcon.c
	@gcc -c prodcon.c

tands.o: tands.c
	@gcc -c tands.c

timer_.o: timer_.c
	@gcc -c timer_.c

clean:
	@rm *.o prodcon