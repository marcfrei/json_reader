/*
** Copyright (c) 2015, Yaler GmbH, Oberon microsystems AG, Switzerland
** All rights reserved
*/

#include <assert.h>

#include "json_reader.h"

#define CHAR_HORIZONTAL_TAB 0x9
#define CHAR_LINE_FEED 0xa
#define CHAR_CARRIAGE_RETURN 0xd
#define CHAR_SPACE 0x20
#define CHAR_QUOTATION_MARK 0x22
#define CHAR_COMMA 0x2c
#define CHAR_DECIMAL_POINT 0x2e
#define CHAR_MINUS_SIGN 0x2d
#define CHAR_DIGIT_ZERO 0x30
#define CHAR_DIGIT_ONE 0x31
#define CHAR_DIGIT_TWO 0x32
#define CHAR_DIGIT_THREE 0x33
#define CHAR_DIGIT_FOUR 0x34
#define CHAR_DIGIT_FIVE 0x35
#define CHAR_DIGIT_SIX 0x36
#define CHAR_DIGIT_SEVEN 0x37
#define CHAR_DIGIT_EIGHT 0x38
#define CHAR_DIGIT_NINE 0x39
#define CHAR_COLON 0x3a
#define CHAR_CAPITAL_LETTER_E 0x45
#define CHAR_LEFT_SQUARE_BRACKET 0x5b
#define CHAR_REVERSE_SOLIDUS 0x5c
#define CHAR_RIGHT_SQUARE_BRACKET 0x5d
#define CHAR_SMALL_LETTER_A 0x61
#define CHAR_SMALL_LETTER_E 0x65
#define CHAR_SMALL_LETTER_F 0x66
#define CHAR_SMALL_LETTER_L 0x6c
#define CHAR_SMALL_LETTER_N 0x6e
#define CHAR_SMALL_LETTER_R 0x72
#define CHAR_SMALL_LETTER_S 0x73
#define CHAR_SMALL_LETTER_T 0x74
#define CHAR_SMALL_LETTER_U 0x75
#define CHAR_LEFT_CURLY_BRACKET 0x7b
#define CHAR_RIGHT_CURLY_BRACKET 0x7d

#define SUBSTATE_NONE 0

#define SUBSTATE_READING_STRING_AFTER_ESCAPE 1

#define SUBSTATE_READING_FALSE_AFTER_F 1
#define SUBSTATE_READING_FALSE_AFTER_FA 2
#define SUBSTATE_READING_FALSE_AFTER_FAL 3
#define SUBSTATE_READING_FALSE_AFTER_FALS 4

#define SUBSTATE_READING_TRUE_AFTER_T 1
#define SUBSTATE_READING_TRUE_AFTER_TR 2
#define SUBSTATE_READING_TRUE_AFTER_TRU 3

#define SUBSTATE_READING_NULL_AFTER_N 1
#define SUBSTATE_READING_NULL_AFTER_NU 2
#define SUBSTATE_READING_NULL_AFTER_NUL 3

void json_reader_init (struct json_reader* r) {
	assert(r != 0);
	assert(r != 0);
	r->state = JSON_READER_STATE_READING_WHITESPACE;
	r->substate = SUBSTATE_NONE;
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
					while ((x == CHAR_SPACE) || (x == CHAR_HORIZONTAL_TAB)
						|| (x == CHAR_LINE_FEED) || (x == CHAR_CARRIAGE_RETURN))
					{
						n++;
						if (n != length) {
							x = buffer[n];
						} else {
							x = -1;
						}
					}
					switch (x) {
						case CHAR_LEFT_CURLY_BRACKET:
							n++;
							r->state = JSON_READER_STATE_BEGINNING_OBJECT;
							break;
						case CHAR_LEFT_SQUARE_BRACKET:
							n++;
							r->state = JSON_READER_STATE_BEGINNING_ARRAY;
							break;
						case CHAR_MINUS_SIGN:
							n++;
							r->state = JSON_READER_STATE_READING_NUMBER;
							break;
						case CHAR_DIGIT_ZERO:
							n++;
							r->state = JSON_READER_STATE_READING_NUMBER;
							r->substate = 2;
							break;
						case CHAR_DIGIT_ONE:
						case CHAR_DIGIT_TWO:
						case CHAR_DIGIT_THREE:
						case CHAR_DIGIT_FOUR:
						case CHAR_DIGIT_FIVE:
						case CHAR_DIGIT_SIX:
						case CHAR_DIGIT_SEVEN:
						case CHAR_DIGIT_EIGHT:
						case CHAR_DIGIT_NINE:
							n++;
							r->state = JSON_READER_STATE_READING_NUMBER;
							r->substate = 1;
							break;
						case CHAR_QUOTATION_MARK:
							n++;
							r->state = JSON_READER_STATE_READING_STRING;
							break;
						case CHAR_SMALL_LETTER_F:
							n++;
							r->state = JSON_READER_STATE_READING_FALSE;
							r->substate = SUBSTATE_READING_FALSE_AFTER_F;
							break;
						case CHAR_SMALL_LETTER_T:
							n++;
							r->state = JSON_READER_STATE_READING_TRUE;
							r->substate = SUBSTATE_READING_TRUE_AFTER_T;
							break;
						case CHAR_SMALL_LETTER_N:
							n++;
							r->state = JSON_READER_STATE_READING_NULL;
							r->substate = SUBSTATE_READING_NULL_AFTER_N;
							break;
						case CHAR_COLON:
							n++;
							r->state = JSON_READER_STATE_AFTER_NAME_SEPARATOR;
							break;
						case CHAR_COMMA:
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
							case CHAR_DIGIT_ZERO:
								n++;
								r->substate = 2;
								break;
							case CHAR_DIGIT_ONE:
							case CHAR_DIGIT_TWO:
							case CHAR_DIGIT_THREE:
							case CHAR_DIGIT_FOUR:
							case CHAR_DIGIT_FIVE:
							case CHAR_DIGIT_SIX:
							case CHAR_DIGIT_SEVEN:
							case CHAR_DIGIT_EIGHT:
							case CHAR_DIGIT_NINE:
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
							while ((CHAR_DIGIT_ZERO <= x) && (x <= CHAR_DIGIT_NINE)) {
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
									case CHAR_DECIMAL_POINT:
										n++;
										r->substate = 3;
										break;
									case CHAR_SMALL_LETTER_E:
									case CHAR_CAPITAL_LETTER_E:
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
					while ((x != -1)
						&& ((x != CHAR_QUOTATION_MARK) || (r->substate != SUBSTATE_NONE)))
					{
						switch (r->substate) {
							case SUBSTATE_NONE:
								if (x == CHAR_REVERSE_SOLIDUS) {
									r->substate = SUBSTATE_READING_STRING_AFTER_ESCAPE;
								}
								break;
							case SUBSTATE_READING_STRING_AFTER_ESCAPE:
								r->substate = SUBSTATE_NONE;
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
						case SUBSTATE_READING_FALSE_AFTER_F:
							if (buffer[n] == CHAR_SMALL_LETTER_A) {
								n++;
								r->substate = SUBSTATE_READING_FALSE_AFTER_FA;
							} else {
								r->state = JSON_READER_STATE_ERROR;
								r->substate = SUBSTATE_NONE;
							}
							break;
						case SUBSTATE_READING_FALSE_AFTER_FA:
							if (buffer[n] == CHAR_SMALL_LETTER_L) {
								n++;
								r->substate = SUBSTATE_READING_FALSE_AFTER_FAL;
							} else {
								r->state = JSON_READER_STATE_ERROR;
								r->substate = SUBSTATE_NONE;
							}
							break;
						case SUBSTATE_READING_FALSE_AFTER_FAL:
							if (buffer[n] == CHAR_SMALL_LETTER_S) {
								n++;
								r->substate = SUBSTATE_READING_FALSE_AFTER_FALS;
							} else {
								r->state = JSON_READER_STATE_ERROR;
								r->substate = SUBSTATE_NONE;
							}
							break;
						case SUBSTATE_READING_FALSE_AFTER_FALS:
							if (buffer[n] == CHAR_SMALL_LETTER_E) {
								n++;
								r->state = JSON_READER_STATE_COMPLETED_FALSE;
								r->substate = SUBSTATE_NONE;
							} else {
								r->state = JSON_READER_STATE_ERROR;
								r->substate = SUBSTATE_NONE;
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
						case SUBSTATE_READING_TRUE_AFTER_T:
							if (buffer[n] == CHAR_SMALL_LETTER_R) {
								n++;
								r->substate = SUBSTATE_READING_TRUE_AFTER_TR;
							} else {
								r->state = JSON_READER_STATE_ERROR;
								r->substate = SUBSTATE_NONE;
							}
							break;
						case SUBSTATE_READING_TRUE_AFTER_TR:
							if (buffer[n] == CHAR_SMALL_LETTER_U) {
								n++;
								r->substate = SUBSTATE_READING_TRUE_AFTER_TRU;
							} else {
								r->state = JSON_READER_STATE_ERROR;
								r->substate = SUBSTATE_NONE;
							}
							break;
						case SUBSTATE_READING_TRUE_AFTER_TRU:
							if (buffer[n] == CHAR_SMALL_LETTER_E) {
								n++;
								r->state = JSON_READER_STATE_COMPLETED_TRUE;
								r->substate = SUBSTATE_NONE;
							} else {
								r->state = JSON_READER_STATE_ERROR;
								r->substate = SUBSTATE_NONE;
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
						case SUBSTATE_READING_NULL_AFTER_N:
							if (buffer[n] == CHAR_SMALL_LETTER_U) {
								n++;
								r->substate = SUBSTATE_READING_NULL_AFTER_NU;
							} else {
								r->state = JSON_READER_STATE_ERROR;
								r->substate = SUBSTATE_NONE;
							}
							break;
						case SUBSTATE_READING_NULL_AFTER_NU:
							if (buffer[n] == CHAR_SMALL_LETTER_L) {
								n++;
								r->substate = SUBSTATE_READING_NULL_AFTER_NUL;
							} else {
								r->state = JSON_READER_STATE_ERROR;
								r->substate = SUBSTATE_NONE;
							}
							break;
						case SUBSTATE_READING_NULL_AFTER_NUL:
							if (buffer[n] == CHAR_SMALL_LETTER_L) {
								n++;
								r->state = JSON_READER_STATE_COMPLETED_NULL;
								r->substate = SUBSTATE_NONE;
							} else {
								r->state = JSON_READER_STATE_ERROR;
								r->substate = SUBSTATE_NONE;
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
