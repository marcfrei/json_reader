/*
 * Copyright (c) 2016 - 2018, Yaler GmbH, Oberon microsystems AG, Switzerland
 * All rights reserved
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "json_reader.h"
#include "json_reader_utils.h"

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

static void print_value(char *buffer) {
	size_t k, n, length;
	struct json_reader reader;

	assert(buffer != NULL);
	json_reader_init(&reader);
	k = 0; n = 0; length = strlen(buffer);
	while ((n != length) && (reader.state != JSON_READER_STATE_ERROR)) {
		n += json_reader_read(&reader, &buffer[n], 1);
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
	assert(n == length);
	assert((reader.state == JSON_READER_STATE_READING_WHITESPACE)
		|| (reader.state == JSON_READER_STATE_COMPLETED_OBJECT)
		|| (reader.state == JSON_READER_STATE_COMPLETED_ARRAY)
		|| (reader.state == JSON_READER_STATE_COMPLETED_STRING)
		|| (reader.state == JSON_READER_STATE_COMPLETED_FALSE)
		|| (reader.state == JSON_READER_STATE_COMPLETED_TRUE)
		|| (reader.state == JSON_READER_STATE_COMPLETED_NULL));
}

static void skip_value(char *buffer) {
	size_t n, length;
	struct json_reader reader;
	struct json_reader_context reader_context;

	assert(buffer != NULL);
	json_reader_init(&reader);
	json_reader_context_init(&reader_context);
	n = 0; length = strlen(buffer);
	while ((n != length)
		&& (reader_context.state == JSON_READER_CONETXT_STATE_READING_VALUE))
	{
		n += json_reader_utils_skip_value(&reader, &reader_context, &buffer[n], 1);
	}
	assert(n == length);
	assert(reader_context.state == JSON_READER_CONETXT_STATE_COMPLETED_VALUE);
	assert((reader.state == JSON_READER_STATE_COMPLETED_OBJECT)
		|| (reader.state == JSON_READER_STATE_COMPLETED_ARRAY)
		|| (reader.state == JSON_READER_STATE_COMPLETED_STRING)
		|| (reader.state == JSON_READER_STATE_COMPLETED_FALSE)
		|| (reader.state == JSON_READER_STATE_COMPLETED_TRUE)
		|| (reader.state == JSON_READER_STATE_COMPLETED_NULL));
}

int main() {
	char *json_text;

	print_value("null");
	skip_value("null");

	print_value("false");
	skip_value("false");

	print_value("true");
	skip_value("true");

	print_value("\"Hello world!\"");
	skip_value("\"Hello world!\"");

	json_text =
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
		"      }";
	print_value(json_text);
	skip_value(json_text);

	json_text =
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
		"]";
	print_value(json_text);
	skip_value(json_text);

	print_value("42\n");

	skip_value("{}");
  skip_value("{\"0\":7}");
  skip_value("{\"0\":0,\"1\":1,\"2\":2}");
  skip_value("[]");
  skip_value("[7]");
  skip_value("[7,77,777]");
  skip_value("[\"X\"]");
  skip_value("[false]");
  skip_value("[true]");
  skip_value("[null]");
  skip_value(
		"{\"00\":{\"01\":{\"02\":{\"03\":{\"04\":{\"05\":{\"06\":{\"07\":"
		"{\"08\":{\"09\":{\"10\":{\"11\":{\"12\":{\"13\":{\"15\":{\"15\":"
		"{\"16\":{\"17\":{\"18\":{\"19\":{\"20\":{\"21\":{\"22\":{\"23\":"
		"{\"24\":{\"25\":{\"26\":{\"27\":{\"28\":{\"29\":{\"30\":{\"31\":"
		"{\"32\":{\"33\":{\"34\":{\"35\":{\"36\":{\"37\":{\"38\":{\"39\":"
		"{\"40\":{\"41\":{\"42\":{\"43\":{\"44\":{\"45\":{\"46\":{\"47\":"
		"{\"48\":{\"49\":{\"50\":{\"51\":{\"52\":{\"53\":{\"54\":{\"55\":"
		"{\"56\":{\"57\":{\"58\":{\"59\":{\"60\":{\"61\":{\"62\":{\"63\":"
		"0}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}");
  skip_value(
		"[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["
		"7,77,777"
		"]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]");

	return 0;
}
