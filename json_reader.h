/*
** Copyright (c) 2015, Yaler GmbH, Oberon microsystems AG, Switzerland
** All rights reserved
*/

#ifndef JSON_READER_H
#define JSON_READER_H

#include <stddef.h>

#define JSON_READER_STATE_READING_WHITESPACE 0

#define JSON_READER_STATE_BEGINNING_OBJECT 1
#define JSON_READER_STATE_COMPLETED_OBJECT 2

#define JSON_READER_STATE_BEGINNING_ARRAY 3
#define JSON_READER_STATE_COMPLETED_ARRAY 4

#define JSON_READER_STATE_READING_NUMBER 5
#define JSON_READER_STATE_COMPLETED_NUMBER 6

#define JSON_READER_STATE_READING_STRING 7
#define JSON_READER_STATE_COMPLETED_STRING 8

#define JSON_READER_STATE_READING_FALSE 9
#define JSON_READER_STATE_COMPLETED_FALSE 10

#define JSON_READER_STATE_READING_TRUE 11
#define JSON_READER_STATE_COMPLETED_TRUE 12

#define JSON_READER_STATE_READING_NULL 13
#define JSON_READER_STATE_COMPLETED_NULL 14

#define JSON_READER_STATE_AFTER_NAME_SEPARATOR 15
#define JSON_READER_STATE_AFTER_VALUE_SEPARATOR 16

#define JSON_READER_STATE_ERROR 17

struct json_reader {
	/** readonly **/
	int state;
	int substate;
};

extern void json_reader_init (
	struct json_reader* r);
extern size_t json_reader_read (
	struct json_reader* r, char* buffer, size_t length);

#endif
