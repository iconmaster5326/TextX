# vars
ifeq ($(OS),Windows_NT)
EXE_SUFFIX := .exe
else
LINUX_LINKFLAGS := -ltinfo -lgpm
endif

CXX_FILES := $(wildcard src/*.cpp)
HXX_FILES := $(wildcard src/*.hpp)
O_FILES := $(patsubst src/%.cpp,build/%.o,$(CXX_FILES))

# executables
# (main exe must be at top, so it's default)
textx$(EXE_SUFFIX): $(O_FILES)
	$(CXX) -g -o textx $(O_FILES) -lncurses
	
key_tester$(EXE_SUFFIX): src/key_tester/key_tester.cpp src/curses.hpp
	$(CXX) -g -std=c++03 -Isrc -o key_tester src/key_tester/key_tester.cpp -lncurses

# to do it statically:
#	$(CXX) -g -static -o textx $(O_FILES) -lncurses $(LINUX_LINKFLAGS)

# other rules
$(O_FILES): $(HXX_FILES)
$(O_FILES): | build
$(O_FILES): build/%.o: src/%.cpp
	$(CXX) -g -std=c++03 -Isrc -c -o $@ $<

build:
	mkdir build

# phony rules
all: $(EXE_NAME) key_tester$(EXE_SUFFIX)

clean:
	rm -rf build

spotless: clean
	rm -rf textx textx.exe key_tester key_tester.exe

.PHONY: clean spotless all
