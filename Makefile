# vars
ifeq ($(OS),Windows_NT)
EXE_SUFFIX := .exe
# else
# LINUX_LINKFLAGS := -ltinfo -lgpm
endif

CXX_FILES := $(wildcard src/*.cpp)
HXX_FILES := $(wildcard src/*.hpp)
O_FILES := $(patsubst src/%.cpp,build/%.o,$(CXX_FILES))
D_FILES := $(patsubst src/%.cpp,build/%.d,$(CXX_FILES))
EXE_FILES := textx key_tester textx_tests

CXXFLAGS := -g -std=c++03 -Isrc
LINKFLAGS := -lncurses

# executables
# (main exe must be at top, so it's default)
textx$(EXE_SUFFIX): $(O_FILES)
	$(CXX) $(CXXFLAGS) -o textx $(O_FILES) $(LINKFLAGS)
	
key_tester$(EXE_SUFFIX): src/key_tester/key_tester.cpp src/curses.hpp
	$(CXX) $(CXXFLAGS) -o key_tester src/key_tester/key_tester.cpp $(LINKFLAGS)

textx_tests$(EXE_SUFFIX): test/test_buffer.cpp build/buffer.o src/buffer.hpp
	$(CXX) $(CXXFLAGS) -o textx_tests test/test_buffer.cpp build/buffer.o $(LINKFLAGS)

# to do it statically:
#	$(CXX) -g -static -o textx $(O_FILES) -lncurses $(LINUX_LINKFLAGS)

# other rules
$(D_FILES): | build
$(D_FILES): build/%.d: src/%.cpp
	$(CXX) -MM -MT '$(patsubst %.d,%.o,$@) $@' $(CXXFLAGS) $< > $@
include $(D_FILES)

$(O_FILES): | build
$(O_FILES): build/%.o: build/%.d
	$(CXX) $(CXXFLAGS) -c -o $@ $(patsubst build/%.d,src/%.cpp,$<)

build:
	mkdir build

# phony rules
all: $(patsubst %,%$(EXE_SUFFIX),$(EXE_FILES))

clean:
	rm -rf build

spotless: clean
	rm -rf $(EXE_FILES) $(patsubst %,%.exe,$(EXE_FILES))

.PHONY: clean spotless all
