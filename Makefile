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
	@chmod +x bm
	@cp bm $(HOME)/bin/
	@echo "bm installed to $(HOME)/bin/"
	@echo "Make sure $(HOME)/bin is in your PATH"
	@echo ""
	@echo "If $(HOME)/bin is not in your PATH, add this line to your shell config:"
	@echo "   export PATH=\"\$$PATH:\$$HOME/bin\""
	@echo ""
	@echo "Then run: source ~/.bashrc or source ~/.zshrc (or restart your terminal)"

clean:
	rm -f *.o bm