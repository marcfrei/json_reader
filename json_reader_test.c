/*
** Copyright (c) 2016, Yaler GmbH, Switzerland
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

static char *state(int state) {
	switch (state) {
		case JSON_READER_STATE_READING_WHITESPACE:
			return "JSON_READER_STATE_READING_WHITESPACE";
		case JSON_READER_STATE_BEGINNING_OBJECT:
			return "JSON_READER_STATE_BEGINNING_OBJECT";
		case JSON_READER_STATE_COMPLETED_OBJECT:
			return "JSON_READER_STATE_COMPLETED_OBJECT";
		case JSON_READER_STATE_BEGINNING_ARRAY:
			return "JSON_READER_STATE_BEGINNING_ARRAY";
		case JSON_READER_STATE_COMPLETED_ARRAY:
			return "JSON_READER_STATE_COMPLETED_ARRAY";
		case JSON_READER_STATE_BEGINNING_NUMBER:
			return "JSON_READER_STATE_BEGINNING_NUMBER";
		case JSON_READER_STATE_READING_NUMBER:
			return "JSON_READER_STATE_READING_NUMBER";
		case JSON_READER_STATE_COMPLETED_NUMBER:
			return "JSON_READER_STATE_COMPLETED_NUMBER";
		case JSON_READER_STATE_BEGINNING_STRING:
			return "JSON_READER_STATE_BEGINNING_STRING";
		case JSON_READER_STATE_READING_STRING:
			return "JSON_READER_STATE_READING_STRING";
		case JSON_READER_STATE_COMPLETED_STRING:
			return "JSON_READER_STATE_COMPLETED_STRING";
		case JSON_READER_STATE_BEGINNING_FALSE:
			return "JSON_READER_STATE_BEGINNING_FALSE";
		case JSON_READER_STATE_READING_FALSE:
			return "JSON_READER_STATE_READING_FALSE";
		case JSON_READER_STATE_COMPLETED_FALSE:
			return "JSON_READER_STATE_COMPLETED_FALSE";
		case JSON_READER_STATE_BEGINNING_TRUE:
			return "JSON_READER_STATE_BEGINNING_TRUE";
		case JSON_READER_STATE_READING_TRUE:
			return "JSON_READER_STATE_READING_TRUE";
		case JSON_READER_STATE_COMPLETED_TRUE:
			return "JSON_READER_STATE_COMPLETED_TRUE";
		case JSON_READER_STATE_BEGINNING_NULL:
			return "JSON_READER_STATE_BEGINNING_NULL";
		case JSON_READER_STATE_READING_NULL:
			return "JSON_READER_STATE_READING_NULL";
		case JSON_READER_STATE_COMPLETED_NULL:
			return "JSON_READER_STATE_COMPLETED_NULL";
		case JSON_READER_STATE_AFTER_NAME_SEPARATOR:
			return "JSON_READER_STATE_AFTER_NAME_SEPARATOR";
		case JSON_READER_STATE_AFTER_VALUE_SEPARATOR:
			return "JSON_READER_STATE_AFTER_VALUE_SEPARATOR";
		case JSON_READER_STATE_ERROR:
			return "JSON_READER_STATE_ERROR";
		default:
			assert(0);
			return 0;
	}
}

int main() {
	size_t k, n;
	int length;
	char buffer[512];
	struct json_reader reader;

	length = snprintf(buffer, sizeof buffer,
		/*
		"null"

		"false"

		"true"

		"42\n"

		"\"Hello world!\""

		"      {"
		"        \"Image\": {"
		"            \"Width\":  800,"
		"            \"Height\": 600,"
		"            \"Title\":  \"View from 15th Floor\","
		"            \"Thumbnail\": {"
		"                \"Url\":    \"http://www.example.com/image/481989943\","
		"                \"Height\": 125,"
		"                \"Width\":  100"
		"            },"
		"            \"Animated\" : false,"
		"            \"IDs\": [116, 943, 234, 38793]"
		"          }"
		"      }"
		*/
		
		"["
		"  {"
		"     \"precision\": \"zip\","
		"     \"Latitude\":  37.7668,"
		"     \"Longitude\": -122.3959,"
		"     \"Address\":   \"\","
		"     \"City\":      \"SAN FRANCISCO\","
		"     \"State\":     \"CA\","
		"     \"Zip\":       \"94107\","
		"     \"Country\":   \"US\""
		"  },"
		"  {"
		"     \"precision\": \"zip\","
		"     \"Latitude\":  37.371991,"
		"     \"Longitude\": -122.026020,"
		"     \"Address\":   \"\","
		"     \"City\":      \"SUNNYVALE\","
		"     \"State\":     \"CA\","
		"     \"Zip\":       \"94085\","
		"     \"Country\":   \"US\""
		"  }"
		"]"
	);
	assert((0 <= length) && ((size_t) length < sizeof buffer));

	json_reader_init(&reader);

	n = 0;
	while ((reader.state != JSON_READER_STATE_ERROR) && (n != (size_t) length)) {
		n += json_reader_read(&reader, &buffer[n], (size_t) 1);
		puts(state(reader.state));
		switch (reader.state) {
			case JSON_READER_STATE_BEGINNING_STRING:
			case JSON_READER_STATE_BEGINNING_NUMBER:
				k = n;
				break;
			case JSON_READER_STATE_COMPLETED_STRING:
			case JSON_READER_STATE_COMPLETED_NUMBER:
				putchar(' ');
				putchar(' ');
				while (k != n) {
					putchar(buffer[k]);
					k++;
				}
				putchar('\n');
				break;
		}
	}

	exit(EXIT_SUCCESS);
}
