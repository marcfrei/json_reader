/*
** Copyright (c) 2015, Yaler GmbH, Oberon microsystems AG, Switzerland
** All rights reserved
*/

#include <assert.h>

#include "json_reader.h"

void json_reader_init (struct json_reader* r) {
	assert(r != 0);
	assert(r != 0);
	r->state = JSON_READER_STATE_READING_WHITESPACE;
	r->substate = 0;
	r->result_token = 0;
	r->result_length = 0;
}

static int is_whitespace (int c) {
	return (c == 0x20) || (c == 0x9) || (c == 0xa) || (c == 0xd);
}

static size_t skip_whitespace (char* buffer, size_t length) {
	size_t n;
	assert(buffer != 0);
	n = 0;
	while ((n != length) && is_whitespace(buffer[n])) {
		n++;
	}
	return n;
}

size_t json_reader_read (struct json_reader* r, char* buffer, size_t length) {
	size_t n;
	assert(r != 0);
	assert(buffer != 0);
	r->result_token = 0;
	r->result_length = 0;
	n = 0;
	if (n != length) {
		do {
			switch (r->state) {
				case JSON_READER_STATE_READING_WHITESPACE:
					n += skip_whitespace(&buffer[n], length - n);
					if (n != length) {
						switch (buffer[n]) {
							case 0x7b:
								n++;
								r->state = JSON_READER_STATE_BEGINNING_OBJECT;
								break;
							case 0x5b:
								n++;
								r->state = JSON_READER_STATE_BEGINNING_ARRAY;
								break;
							case 0x2d:
							case 0x31:
							case 0x32:
							case 0x33:
							case 0x34:
							case 0x35:
							case 0x36:
							case 0x37:
							case 0x38:
							case 0x39:
								n++;
								r->state = JSON_READER_STATE_READING_NUMBER;
								break;
							case 0x22:
								n++;
								r->state = JSON_READER_STATE_READING_STRING;
								break;
							case 0x66:
								n++;
								r->state = JSON_READER_STATE_READING_FALSE;
								r->substate = 1;
								break;
							case 0x74:
								n++;
								r->state = JSON_READER_STATE_READING_TRUE;
								r->substate = 1;
								break;
							case 0x6e:
								n++;
								r->state = JSON_READER_STATE_READING_NULL;
								r->substate = 1;
								break;
							case 0x3a:
								n++;
								r->state = JSON_READER_STATE_AFTER_NAME_SEPARATOR;
								break;
							case 0x2c:
								n++;
								r->state = JSON_READER_STATE_AFTER_VALUE_SEPARATOR;
								break;
							default:
								r->state = JSON_READER_STATE_ERROR;
								break;
						}
					}
					break;
				case JSON_READER_STATE_BEGINNING_OBJECT:
					r->state = JSON_READER_STATE_READING_WHITESPACE;
					break;
				case JSON_READER_STATE_COMPLETED_OBJECT:
					r->state = JSON_READER_STATE_READING_WHITESPACE;
					break;
				case JSON_READER_STATE_BEGINNING_ARRAY:
					r->state = JSON_READER_STATE_READING_WHITESPACE;
					break;
				case JSON_READER_STATE_COMPLETED_ARRAY:
					r->state = JSON_READER_STATE_READING_WHITESPACE;
					break;
				case JSON_READER_STATE_READING_NUMBER:
assert(0);
					break;
				case JSON_READER_STATE_COMPLETED_NUMBER:
					r->state = JSON_READER_STATE_READING_WHITESPACE;
					break;
				case JSON_READER_STATE_READING_STRING:
assert(0);
					break;
				case JSON_READER_STATE_COMPLETED_STRING:
					r->state = JSON_READER_STATE_READING_WHITESPACE;
					break;
				case JSON_READER_STATE_READING_FALSE:
					switch (r->substate) {
						case 1:
							if (buffer[n] == ) {
								n++;
								r->substate = 2;
							} else {
								r->state = JSON_READER_STATE_ERROR;
								r->substate = 0;
							}
							break;
						case 2:
assert(0);
							break;
						case 3:
assert(0);
							break;
						case 4:
assert(0);
							break;
						default:
							assert(0);
							break;
					}
					break;
				case JSON_READER_STATE_COMPLETED_FALSE:
					r->state = JSON_READER_STATE_READING_WHITESPACE;
					break;
				case JSON_READER_STATE_READING_TRUE:
assert(0);
					break;
				case JSON_READER_STATE_COMPLETED_TRUE:
					r->state = JSON_READER_STATE_READING_WHITESPACE;
					break;
				case JSON_READER_STATE_READING_NULL:
assert(0);
					break;
				case JSON_READER_STATE_COMPLETED_NULL:
					r->state = JSON_READER_STATE_READING_WHITESPACE;
					break;
				case JSON_READER_STATE_AFTER_NAME_SEPARATOR:
					r->state = JSON_READER_STATE_READING_WHITESPACE;
					break;
				case JSON_READER_STATE_AFTER_VALUE_SEPARATOR:
					r->state = JSON_READER_STATE_READING_WHITESPACE;
					break;
				case JSON_READER_STATE_DONE:
				case JSON_READER_STATE_ERROR:
					break;
				default:
					assert(0);
					break;
			}
		} while ((n != length)
			&& (r->state != JSON_READER_STATE_BEGINNING_OBJECT)
			&& (r->state != JSON_READER_STATE_COMPLETED_OBJECT)
			&& (r->state != JSON_READER_STATE_BEGINNING_ARRAY)
			&& (r->state != JSON_READER_STATE_COMPLETED_ARRAY)
			&& (r->state != JSON_READER_STATE_COMPLETED_NUMBER)
			&& (r->state != JSON_READER_STATE_COMPLETED_STRING)
			&& (r->state != JSON_READER_STATE_COMPLETED_FALSE)
			&& (r->state != JSON_READER_STATE_COMPLETED_TRUE)
			&& (r->state != JSON_READER_STATE_COMPLETED_NULL)
			&& (r->state != JSON_READER_STATE_AFTER_NAME_SEPARATOR)
			&& (r->state != JSON_READER_STATE_AFTER_VALUE_SEPARATOR)
			&& (r->state != JSON_READER_STATE_DONE)
			&& (r->state != JSON_READER_STATE_ERROR));
	}
	return n;
}
