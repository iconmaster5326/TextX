ifeq ($(OS),Windows_NT)
    EXE_SUFFIX := .exe
endif

EXE_NAME := textx$(EXE_SUFFIX)

$(EXE_NAME): src/textx.cpp src/curses.hpp
	$(CXX) -Isrc -o textx src/textx.cpp -lncurses

all: $(EXE_NAME)

clean:
	$(RM) textx textx.exe

run: $(EXE_NAME)
	./$(EXE_NAME)

.PHONY: clean run all
