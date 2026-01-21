CFLAGS = -Wall -Wextra -g

all: bm

bm: main.o bookmarks.o
	gcc $(CFLAGS) main.o bookmarks.o -o bm

main.o: src/main.c
	gcc $(CFLAGS) -c src/main.c -o main.o

bookmarks.o: src/bookmarks.c
	gcc $(CFLAGS) -c src/bookmarks.c -o bookmarks.o

install: bm
	@mkdir -p $(HOME)/bin
	@cp bm $(HOME)/bin/
	@echo "bm installed to $(HOME)/bin/"
	@echo "Make sure $(HOME)/bin is in your PATH"

clean:
	rm -f *.o bm