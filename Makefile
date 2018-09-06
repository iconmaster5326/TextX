textx.exe: src/textx.cpp
	$(CXX) -o textx src/textx.cpp -lncurses

all: textx.exe

clean:
	$(RM) textx

run: textx.exe
	./textx

.PHONY: clean run all
