OPTION = -Wall -ggdb -Wl,-Ttext-segment=0x20000000
OPTION2 = -Wall -Wextra -g -O0 -I ./include
CFLAGS =-g

all : apager dpager hpager test

apager : apager.o
	gcc $(OPTION) -o apager $<

apager.o : ./apager.c ./include/apager.h
	gcc $(OPTION2) -c $<

dpager : dpager.o
	gcc $(OPTION) -o dpager $<

dpager.o : ./dpager.c ./include/dpager.h
	gcc  $(OPTION2) -c $<

hpager : hpager.o
	gcc $(OPTION) -o hpager $<

hpager.o : ./hpager.c ./include/hpager.h
	gcc $(OPTION2) -c $<

test:
	gcc test1.c -g -O0 -o test1 -static -std=c99
	gcc test2.c -g -O0 -o test2 -static -std=c99
	gcc test3.c -g -O0 -o test3 -static -std=c99

clean:
	rm -f apager.o
	rm -f apager
	rm -f dpager.o
	rm -f dpager
	rm -f hpager.o
	rm -f hpager
	rm -f test1
	rm -f test2
	rm -f test3
	rm -f test4
