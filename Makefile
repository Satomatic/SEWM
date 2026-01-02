PREFIX?=/usr/X11R6
CFLAGS?=-std=c++11

all:
	g++ $(CFLAGS) -I$(PREFIX)/include src/*.cpp src/**/*.cpp -Isrc/include -L$(PREFIX)/lib -lX11 -o sewm -O2

clean:
	rm -f sewm

