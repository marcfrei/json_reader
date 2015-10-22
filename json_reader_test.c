/*
** Copyright (c) 2015, Yaler GmbH, Switzerland
** All rights reserved
*/

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "json_reader.h"

#if !(defined __APPLE__ && defined __MACH__)
extern int snprintf(char *str, size_t size, char *format, ...);
#endif

int main () {
	size_t n;
	int length;
	char buffer[256];
	struct json_reader reader;

	length = snprintf(buffer, sizeof buffer, "");
	assert((0 <= length) && ((size_t) length < sizeof buffer));

	json_reader_init(&reader);
	n = json_reader_read(&reader, buffer, (size_t) length);

	exit(EXIT_SUCCESS);
}
