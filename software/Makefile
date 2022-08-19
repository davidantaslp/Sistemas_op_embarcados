C_FLAGS=-lwiringPi -lpigpio
LFLAGS=-L/usr/local/lib/
soe : main.o gps.o
	gcc $(CFLAGS) -o soe main.o gps.o $(LFLAGS) $(C_FLAGS)
# main.o: main.c
# 	gcc $(CFLAGS) -c main.c $(C_FLAGS)
# gps.o: gps.c gps.h
# 	gcc $(CFLAGS) -c gps.c $(C_FLAGS)
clean:
	rm -f *. soe
