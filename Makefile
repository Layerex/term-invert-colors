CXXFLAGS=-std=c++98 -Wall -Wextra -O3
LDLIBS=-lansiescape

all: term-invert-colors

install: all
	cp term-invert-colors /usr/local/bin

clean:
	rm -f term-invert-colors

.PHONY: all install clean
