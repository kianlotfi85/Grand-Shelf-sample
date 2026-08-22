# Offline fallback build: plain g++ + make, no cmake and no network required.
# The CMake build is the primary path once cmake and the optional libraries are
# available; this Makefile exists so the project can be compiled and tested on a
# bare toolchain.

CXX      ?= g++
BUILD    ?= build-make
SRC_DIR  := src
TEST_DIR := tests

CXXFLAGS ?= -std=c++17 -O2 -g -Wall -Wextra -Wpedantic -pthread
INCLUDES := -I$(SRC_DIR)

# Use the system sqlite3 header when present, otherwise the vendored subset.
SQLITE_HEADER := $(firstword $(wildcard /usr/include/sqlite3.h /usr/local/include/sqlite3.h))
ifeq ($(SQLITE_HEADER),)
INCLUDES += -Ithird_party/sqlite3
endif

# Link against whichever libsqlite3 exists, including a versioned-only install.
SQLITE_LIB := $(firstword $(wildcard \
  /usr/lib/x86_64-linux-gnu/libsqlite3.so \
  /usr/lib/x86_64-linux-gnu/libsqlite3.so.0 \
  /usr/lib/aarch64-linux-gnu/libsqlite3.so.0 \
  /usr/lib/libsqlite3.so.0 \
  /usr/local/lib/libsqlite3.so))

LDLIBS := $(SQLITE_LIB) -pthread

SOURCES := $(shell find $(SRC_DIR) -name '*.cpp' -not -name 'main.cpp')
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD)/obj/%.o,$(SOURCES))
MAIN_OBJ := $(BUILD)/obj/main.o
BIN := $(BUILD)/study-planner

TEST_SOURCES := $(wildcard $(TEST_DIR)/*.cpp)
TEST_BINS := $(patsubst $(TEST_DIR)/%.cpp,$(BUILD)/%,$(TEST_SOURCES))

.PHONY: all check-sqlite tests test clean

all: check-sqlite $(BIN)

check-sqlite:
ifeq ($(SQLITE_LIB),)
	$(error No libsqlite3 shared library found. Install libsqlite3-0 (runtime) or libsqlite3-dev, or set SQLITE_LIB=/path/to/libsqlite3.so)
endif

$(BIN): $(OBJECTS) $(MAIN_OBJ)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDLIBS)

$(BUILD)/obj/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

$(BUILD)/%: $(TEST_DIR)/%.cpp $(OBJECTS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -I$(TEST_DIR) $< $(OBJECTS) -o $@ $(LDLIBS)

tests: check-sqlite $(TEST_BINS)

test: tests
	@status=0; for t in $(TEST_BINS); do echo "running $$t"; "$$t" || status=1; done; exit $$status

clean:
	rm -rf $(BUILD)

-include $(shell find $(BUILD) -name '*.d' 2>/dev/null)
