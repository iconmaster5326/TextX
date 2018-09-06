textx.exe: src/textx.cpp src/curses.hpp
	$(CXX) -Isrc -o textx src/textx.cpp -lncurses

all: textx.exe

clean:
	$(RM) textx

run: textx.exe
	./textx

.PHONY: clean run all
