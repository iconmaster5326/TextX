ifeq ($(OS),Windows_NT)
EXE_SUFFIX := .exe
else
LINUX_LINKFLAGS := -ltinfo -lgpm
endif

EXE_NAME := textx$(EXE_SUFFIX)

$(EXE_NAME): src/textx.cpp src/curses.hpp src/pane.cpp src/pane.hpp src/app.cpp src/app.hpp
	$(CXX) -static -std=c++11 -Isrc -o textx src/textx.cpp src/pane.cpp src/app.cpp -lncurses $(LINUX_LINKFLAGS)

all: $(EXE_NAME)

clean:
	$(RM) textx textx.exe

run: $(EXE_NAME)
	./$(EXE_NAME)

.PHONY: clean run all
