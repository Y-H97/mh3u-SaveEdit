# Build & run the MH3U SaveEdit unit tests.
#
#   make          # build and run the tests
#   make test     # same as above
#   make coverage # build with gcov instrumentation, run, print line coverage
#   make clean    # remove build artifacts
#
# The tests only cover the non-GUI core logic (MH3U_Armory, MH3U_SE, MH3U_DS),
# so no Qt dependency is required.

CXX      ?= g++
CXXFLAGS ?= -std=c++11 -Wall -Wextra -g -O0

SRC_DIR := ../source/app/MH3U_Save_Editor

INCLUDES := -I. -I$(SRC_DIR)

# Core (production) sources under test.
CORE_SRCS := \
	$(SRC_DIR)/mh3u_se.cpp \
	$(SRC_DIR)/mh3u_ds.cpp

# Test sources.
TEST_SRCS := \
	test_framework.cpp \
	test_armory.cpp \
	test_se.cpp \
	test_ds.cpp

SRCS := $(CORE_SRCS) $(TEST_SRCS)
BIN  := run_tests

.PHONY: all test coverage clean

all: test

$(BIN): $(SRCS) test_framework.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRCS) -o $(BIN)

test: $(BIN)
	./$(BIN)

coverage: CXXFLAGS += --coverage
coverage: clean $(BIN)
	./$(BIN)
	@echo ""
	@echo "===== gcov line coverage (core modules) ====="
	@gcov -n $(BIN)-mh3u_se $(BIN)-mh3u_ds 2>/dev/null \
		| grep -A1 -E "MH3U_Save_Editor/mh3u_(se|ds)\.cpp'" || true

clean:
	rm -f $(BIN) *.o *.gcno *.gcda *.gcov
	rm -f mh3u_test_*.sav
	rm -rf mh3u_ds_tmp
