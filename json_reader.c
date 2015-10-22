/*
** Copyright (c) 2015, Yaler GmbH, Switzerland
** All rights reserved
*/

#include <assert.h>

#include "json_reader.h"


void json_reader_init (struct json_reader* r) {
	assert(r != 0);
}

size_t json_reader_read (struct json_reader* r, char* buffer, size_t length) {
	assert(r != 0);
	assert(buffer != 0);
	(void) length;
	return 0;
}
