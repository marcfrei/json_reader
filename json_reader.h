/*
** Copyright (c) 2015, Yaler GmbH, Switzerland
** All rights reserved
*/

#ifndef JSON_READER_H
#define JSON_READER_H

#include <stddef.h>

struct json_reader {
	char *result_token;
	size_t result_length;
};

extern void json_reader_init (
	struct json_reader* r);
extern size_t json_reader_read (
	struct json_reader* r, char* buffer, size_t length);

#endif
