default: json_reader_test

CC = gcc
CFLAGS = -ansi -pedantic -g -O3 \
	-Wall -Wextra -Wc++-compat -Wno-unknown-pragmas \
	-O3 -fno-strict-overflow -fno-strict-aliasing

ifeq (, $(findstring clang, $(shell gcc --version)))
	CFLAGS += -fno-delete-null-pointer-checks
endif

CC-OBJECT = $(CC) $(CFLAGS) -c $(filter-out %.h, $^)
CC-EXECUTABLE = $(CC) $(CFLAGS) -o $@ $(filter-out %.h, $^)

json_reader.o: json_reader.c json_reader.h
	$(CC-OBJECT)

json_reader_test: json_reader_test.c json_reader.o
	$(CC-EXECUTABLE)

clean:
	rm -f json_reader_test
	rm -rf json_reader_test.dSYM
	rm -f json_reader.o

.PHONY: default clean
