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

size_t json_reader_read (struct json_reader* r, char* buffer, size_t length) {
	size_t n; int x;
	assert(r != 0);
	assert(buffer != 0);
	r->result_token = 0;
	r->result_length = 0;
	n = 0;
	if (n != length) {
		do {
			switch (r->state) {
				case JSON_READER_STATE_READING_WHITESPACE:
					x = buffer[n];
					while ((x == 0x20) || (x == 0x9) || (x == 0xa) || (x == 0xd)) {
						n++;
						if (n != length) {
							x = buffer[n];
						} else {
							x = -1;
						}
					}
					switch (x) {
						case 0x7b:
							n++;
							r->state = JSON_READER_STATE_BEGINNING_OBJECT;
							break;
						case 0x5b:
							n++;
							r->state = JSON_READER_STATE_BEGINNING_ARRAY;
							break;
						case 0x2d:
							n++;
							r->state = JSON_READER_STATE_READING_NUMBER;
							break;
						case 0x30:
							n++;
							r->state = JSON_READER_STATE_READING_NUMBER;
							r->substate = 2;
							break;
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
							r->substate = 1;
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
						case -1:
							break;
						default:
							r->state = JSON_READER_STATE_ERROR;
							break;
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
					if (r->substate == 0) {
						switch(buffer[n]) {
							case 0x30:
								n++;
								r->substate = 2;
								break;
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
								r->substate = 1;
								break;
							default:
								r->state = JSON_READER_STATE_ERROR;
						}
					}
					if (n != length) {
						if (r->substate == 1) {
							x = buffer[n];
							while ((0x30 <= x) && (x <= 0x39)) {
								n++;
								if (n != length) {
									x = buffer[n];
								} else {
									x = -1;
								}
							}
							r->substate = 2;
						}
						if (n != length) {
							if (r->substate == 2) {
								switch (buffer[n]) {
									case 0x2e:
										n++;
										r->substate = 3;
										break;
									case 0x65:
									case 0x45:
										n++;
										r->substate = 4;
										break;
									default:
										r-> substate = 5;
								}
							}
							if (n != length) {
								if (r->substate == 3) {
assert(0);
								}
assert(0);
							}
						}
					}
					break;
				case JSON_READER_STATE_COMPLETED_NUMBER:
					r->state = JSON_READER_STATE_READING_WHITESPACE;
					break;
				case JSON_READER_STATE_READING_STRING:
					x = buffer[n];
					while ((x != -1) && ((x != 0x22) || (r->substate != 0))) {
						switch (r->substate) {
							case 0:
								if (x == 0x5c) {
									r->substate = 1;
								}
								break;
							case 1:
								r->substate = 0;
								break;
							default:
								assert(0);
								break;
						}
						n++;
						if (n != length) {
							x = buffer[n];
						} else {
							x = -1;
						}
					}
					if (x != -1) {
						r->state = JSON_READER_STATE_COMPLETED_STRING;
					}
					break;
				case JSON_READER_STATE_COMPLETED_STRING:
					r->state = JSON_READER_STATE_READING_WHITESPACE;
					break;
				case JSON_READER_STATE_READING_FALSE:
					switch (r->substate) {
						case 1:
							if (buffer[n] == 0x61) {
								n++;
								r->substate = 2;
							} else {
								r->state = JSON_READER_STATE_ERROR;
								r->substate = 0;
							}
							break;
						case 2:
							if (buffer[n] == 0x6c) {
								n++;
								r->substate = 3;
							} else {
								r->state = JSON_READER_STATE_ERROR;
								r->substate = 0;
							}
							break;
						case 3:
							if (buffer[n] == 0x73) {
								n++;
								r->substate = 4;
							} else {
								r->state = JSON_READER_STATE_ERROR;
								r->substate = 0;
							}
							break;
						case 4:
							if (buffer[n] == 0x65) {
								n++;
								r->state = JSON_READER_STATE_COMPLETED_FALSE;
								r->substate = 0;
							} else {
								r->state = JSON_READER_STATE_ERROR;
								r->substate = 0;
							}
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
					switch (r->substate) {
						case 1:
							if (buffer[n] == 0x72) {
								n++;
								r->substate = 2;
							} else {
								r->state = JSON_READER_STATE_ERROR;
								r->substate = 0;
							}
							break;
						case 2:
							if (buffer[n] == 0x75) {
								n++;
								r->substate = 3;
							} else {
								r->state = JSON_READER_STATE_ERROR;
								r->substate = 0;
							}
							break;
						case 3:
							if (buffer[n] == 0x65) {
								n++;
								r->state = JSON_READER_STATE_COMPLETED_TRUE;
								r->substate = 0;
							} else {
								r->state = JSON_READER_STATE_ERROR;
								r->substate = 0;
							}
							break;
						default:
							assert(0);
							break;
					}
					break;
				case JSON_READER_STATE_COMPLETED_TRUE:
					r->state = JSON_READER_STATE_READING_WHITESPACE;
					break;
				case JSON_READER_STATE_READING_NULL:
					switch (r->substate) {
						case 1:
							if (buffer[n] == 0x75) {
								n++;
								r->substate = 2;
							} else {
								r->state = JSON_READER_STATE_ERROR;
								r->substate = 0;
							}
							break;
						case 2:
							if (buffer[n] == 0x6c) {
								n++;
								r->substate = 3;
							} else {
								r->state = JSON_READER_STATE_ERROR;
								r->substate = 0;
							}
							break;
						case 3:
							if (buffer[n] == 0x6c) {
								n++;
								r->state = JSON_READER_STATE_COMPLETED_NULL;
								r->substate = 0;
							} else {
								r->state = JSON_READER_STATE_ERROR;
								r->substate = 0;
							}
							break;
						default:
							assert(0);
							break;
					}
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
			&& (r->state != JSON_READER_STATE_ERROR));
	}
	return n;
}
