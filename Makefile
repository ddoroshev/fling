VERSION := $(shell git describe --tags --always --dirty)
CC = gcc
CFLAGS = -I. -Wall -std=gnu11 -fno-omit-frame-pointer -O3 -DFLING_VERSION=\"$(VERSION)\"
LDFLAGS =

DEBUG_CFLAGS = -I. -Wall -std=gnu11 -fno-omit-frame-pointer -g
DEBUG_LDFLAGS =

ifeq ("$(DEBUG)","1")
	CFLAGS += -DDEBUG=1
	DEBUG_CFLAGS += -DDEBUG=1
endif

BIN_DIR = ./bin

FLING = $(BIN_DIR)/fling
FLING_DEBUG = $(FLING)_debug
TEST = $(BIN_DIR)/test

SRC_COMMON = client.c file.c fsock.c progress.c server.c
SRC_FLING = main.c receiver.c sender.c $(SRC_COMMON)
SRC_TEST = tests/test.c tests/test_e2e.c tests/test_file.c tests/test_receiver_payload.c $(SRC_COMMON)

OBJ_FLING = $(SRC_FLING:.c=.o)
OBJ_TEST = $(SRC_TEST:.c=.o)

DEPS = $(SRC_FLING:.c=.d) $(SRC_TEST:.c=.d)

.PHONY: all debug clean test testrun test-data info check

all: $(FLING)

$(BIN_DIR):
	mkdir -p $@

$(FLING): $(OBJ_FLING) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(TEST): $(OBJ_TEST) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

debug: CFLAGS = $(DEBUG_CFLAGS)
debug: LDFLAGS = $(DEBUG_LDFLAGS)
debug: clean-debug $(FLING_DEBUG)

$(FLING_DEBUG): $(OBJ_FLING) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -MM -MP -MF $@.$$$$ $(CFLAGS) $<; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

-include $(DEPS)

test: $(TEST)

testrun: test
	$(TEST)

testrun-slow: test
	$(TEST) s

test-data-basic:
	mkdir -p tests/data tests/gen-data
	@if command -v tmutil >/dev/null 2>&1; then \
		tmutil addexclusion tests/data tests/gen-data; \
	fi
	# empty file
	touch tests/gen-data/file-0.dat
	# long file name
	touch "tests/gen-data/$(shell printf 'a%.0s' $$(seq 1 255))"
	dd if=/dev/zero of=tests/gen-data/file-1k.dat bs=1K count=1 status=none
	dd if=/dev/zero of=tests/gen-data/file-1M.dat bs=1M count=1 status=none
	dd if=/dev/zero of=tests/gen-data/file-10M.dat bs=10M count=1 status=none

test-data-slow: test-data-basic
	dd if=/dev/zero of=tests/gen-data/file-100M.dat bs=100M count=1 status=none
	dd if=/dev/zero of=tests/gen-data/file-1G.dat bs=1G count=1 status=none
	dd if=/dev/zero of=tests/gen-data/file-5G.dat bs=1G count=5 status=none
	dd if=/dev/zero of=tests/gen-data/file-10G.dat bs=1G count=10 status=none

test-data: test-data-slow

check:
	cppcheck --enable=all --suppress=missingIncludeSystem .

clean: clean-objs clean-bins

clean-objs:
	rm -f $(OBJ_FLING) $(OBJ_TEST) $(DEPS) $(DEPS:=.*)

clean-bins:
	rm -f $(FLING) $(FLING_DEBUG) $(TEST)

clean-debug:
	rm -f $(FLING_DEBUG)

playground:
	$(CC) -o bin/playground playground.c
	bin/playground

info:
	@echo "Source files (common): $(SRC_COMMON)"
	@echo "Source files (fling): $(SRC_FLING)"
	@echo "Source files (test): $(SRC_TEST)"
	@echo "Object files (fling): $(OBJ_FLING)"
	@echo "Object files (test): $(OBJ_TEST)"
	@echo "Dependencies: $(DEPS)"