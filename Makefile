# vars
ifeq ($(OS),Windows_NT)
EXE_SUFFIX := .exe
else
LINUX_LINKFLAGS := -ltinfo -lgpm
endif

EXE_NAME := textx$(EXE_SUFFIX)

CXX_FILES := $(wildcard src/*.cpp)
HXX_FILES := $(wildcard src/*.hpp)
O_FILES := $(patsubst src/%.cpp,build/%.o,$(CXX_FILES))

# exe must be at top, so it's default
$(EXE_NAME): $(O_FILES)
	$(CXX) -g -static -o textx $(O_FILES) -lncurses $(LINUX_LINKFLAGS)

# other rules
$(O_FILES): build/%.o: src/%.cpp build $(HXX_FILES)
	$(CXX) -g -std=c++03 -Isrc -c -o $@ $<

build:
	mkdir build

# phony rules
all: $(EXE_NAME)

clean:
	rm -rf build textx textx.exe

run: $(EXE_NAME)
	`readlink -f $(EXE_NAME)`

.PHONY: clean run all
