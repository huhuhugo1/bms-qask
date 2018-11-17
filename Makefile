CC=g++
CFLAGS=-std=c++17 -Wall -Wextra -pedantic -O3 -no-pie
LIBS=-lm

all: bms1A bms1B

bms1A: modulator.cpp
	$(CC) $(CFLAGS) $(LIBS) $^ -o $@ libsndfile.a

bms1B: demodulator.cpp
	$(CC) $(CFLAGS) $(LIBS) $^ -o $@ libsndfile.a

clean:
	rm -f *.o bms1A bms1B
