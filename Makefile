default: json_reader_test

SRC = \
	json_reader.c \
	json_reader_utils.c \
	json_reader_test.c

OBJ = $(SRC:.c=.o)

DEP = $(SRC:.c=.d)
-include $(DEP)

CPPFLAGS += -MMD -MP

CFLAGS += -std=c99 -pedantic -g -O2 \
	-fno-strict-overflow -fno-strict-aliasing \
	-Wall -Wextra -Wc++-compat -Wno-unknown-pragmas

ifneq ($(findstring clang, $(shell gcc --version)), clang)
	CFLAGS += -fno-delete-null-pointer-checks
endif

json_reader_test: $(OBJ)

clean:
	rm -f json_reader_test $(OBJ) $(DEP)

.PHONY: default clean
