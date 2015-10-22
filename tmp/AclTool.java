// Copyright (c) 2015, Yaler GmbH, Switzerland
// All rights reserved

import java.io.Closeable;
import java.io.File;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.security.GeneralSecurityException;
import java.security.SecureRandom;
import java.util.ArrayList;
import javax.crypto.SecretKeyFactory;
import javax.crypto.spec.PBEKeySpec;

import org.yaler.core.CdbWriters.CdbWriter;

class AclTool {
	private static final int
		TAB_SIZE = TAB_SIZE(2);

	private static final String
		USAGE = "Usage: java AclTool make <aclfile> < JSON text",

		CREATE_CDB_FILE_ERROR = "AclTool:error:unable to create ACL file",
		INPUT_FORMAT_ERROR = "AclTool:error:unexpected input",
		INPUT_STREAM_ERROR = "AclTool:error:unable to read input",
		WRITE_CDB_FILE_ERROR = "AclTool:error:unable to write ACL file",

		COLON_EXPECTED = "':' expected",
		COMMA_OR_END_OF_ARRAY_EXPECTED = "',' or ']' expected",
		COMMA_OR_END_OF_OBJECT_EXPECTED = "',' or '}' expected",
		DATA_LENGTH_ERROR = "data too long",
		END_OF_ARRAY_EXPECTED = "']' expected",
		END_OF_OBJECT_EXPECTED = "'}' expected",
		END_OF_TEXT_EXPECTED = "end of text expected",
		INVALID_CHARACTER = "invalid character, missing '\"'?",
		INVALID_ESCAPE_SEQUENCE = "invalid escape sequence",
		KEY_LENGTH_ERROR = "key too long",
		NULL_EXPECTED = "'null' expected",
		NULL_OR_ARRAY_EXPECTED = "'null' or '[' expected",
		OBJECT_EXPECTED = "'{' expected",
		STRING_EXPECTED = "'\"' expected",
		STRING_OR_END_OF_ARRAY_EXPECTED = "'\"' or ']' expected",
		STRING_OR_END_OF_OBJECT_EXPECTED = "'\"' or '}' expected",
		UNSUPPORTED_CHARACTER = "unsupported character",
		UNSUPPORTED_ESCAPE_SEQUENCE = "unsupported escape sequence";

	private static final SecureRandom
		secureRandom = new SecureRandom();
	private static final SecretKeyFactory
		secretKeyFactory = newSecretKeyFactory();

	private static int
		currentByte,
		currentLine,
		currentColumn;
	private static RandomAccessFile cdb;
	private static CdbWriter cdbWriter;

	private static int TAB_SIZE (int value) {
		int tabSize = Integer.getInteger("tab-size", value);
		assert tabSize > 0;
		return tabSize;
	}

	private static SecretKeyFactory newSecretKeyFactory () {
		try {
			return SecretKeyFactory.getInstance("PBKDF2WithHmacSHA1");
		} catch (GeneralSecurityException e) {
			throw new Error(e);
		}
	}

	private static void close (Closeable c) {
		try {
			c.close();
		} catch (IOException e) {}
	}

	private static void exit (String msg) {
		System.err.println(msg);
		System.exit(1);
	}

	private static void closeAndExit (Closeable c, String msg) {
		close(c);
		exit(msg);
	}

	private static void signalInputError (String msg) {
		closeAndExit(cdb, INPUT_FORMAT_ERROR +
			":" + currentLine + "," + currentColumn + ": " + msg);
	}

	private static byte[] derivedKey (
		char[] password, byte[] salt, int iterationCount, int keyLength)
	{
		try {
			return secretKeyFactory
				.generateSecret(
					new PBEKeySpec(password, salt, iterationCount, keyLength))
				.getEncoded();
		} catch (GeneralSecurityException e) {
			throw new Error(e);
		}
	}

	private static byte[] salt (int n) {
		byte[] salt = new byte[n];
		secureRandom.nextBytes(salt);
		return salt;
	}

	private static void emit (String domain, ArrayList<String> keys) {
		long keyLength = domain.length();
		if (keyLength > CdbWriter.MAX_FIELD_LENGTH) {
			signalInputError(KEY_LENGTH_ERROR);
		}

		long dataLength = keys == null? 8: 60L * keys.size();
		if (dataLength > CdbWriter.MAX_FIELD_LENGTH) {
			signalInputError(DATA_LENGTH_ERROR);
		}

		try {
			cdbWriter.startRecord(keyLength, dataLength);
			for (int i = 0; i != domain.length(); i++) {
				char c = domain.charAt(i);
				assert c <= Byte.MAX_VALUE;
				cdbWriter.writeKeyByte((byte) c);
			}
			if (keys == null) {
				cdbWriter.writeDataByte(0x01);
				cdbWriter.writeDataByte(0x01);
				cdbWriter.writeDataByte(0x00);
				cdbWriter.writeDataByte(0x00);
				cdbWriter.writeDataByte(0x08);
				cdbWriter.writeDataByte(0x00);
				cdbWriter.writeDataByte(0x00);
				cdbWriter.writeDataByte(0x00);
			} else {
				for (String key: keys) {
					byte[] salt = salt(8);
					byte[] derivedKey = derivedKey(key.toCharArray(), salt, 1024, 256);
					cdbWriter.writeDataByte(0x01);
					cdbWriter.writeDataByte(0x03);
					cdbWriter.writeDataByte(0x00);
					cdbWriter.writeDataByte(0x00);
					cdbWriter.writeDataByte(0x3c);
					cdbWriter.writeDataByte(0x00);
					cdbWriter.writeDataByte(0x00);
					cdbWriter.writeDataByte(0x00);
					cdbWriter.writeDataByte(0x08);
					cdbWriter.writeDataByte(0x00);
					cdbWriter.writeDataByte(0x00);
					cdbWriter.writeDataByte(0x00);
					for (byte x: salt) {
						cdbWriter.writeDataByte(x);
					}
					cdbWriter.writeDataByte(0x00);
					cdbWriter.writeDataByte(0x04);
					cdbWriter.writeDataByte(0x00);
					cdbWriter.writeDataByte(0x00);
					cdbWriter.writeDataByte(0x00);
					cdbWriter.writeDataByte(0x01);
					cdbWriter.writeDataByte(0x00);
					cdbWriter.writeDataByte(0x00);
					for (byte x: derivedKey) {
						cdbWriter.writeDataByte(x);
					}
				}
			}
			cdbWriter.finishRecord();
		} catch (IOException e) {
			closeAndExit(cdb, WRITE_CDB_FILE_ERROR);
		}
	}

	private static int read () {
		if (currentByte != -1) {
			int x = -1;
			try {
				x = System.in.read();
			} catch (IOException e) {
				closeAndExit(cdb, INPUT_STREAM_ERROR);
			}
			if (currentByte == '\n') {
				assert currentLine < Integer.MAX_VALUE;
				currentLine++;
				currentColumn = 1;
			} else if (currentByte == '\t') {
				assert currentColumn <= Integer.MAX_VALUE - TAB_SIZE;
				currentColumn += TAB_SIZE;
			} else {
				assert currentColumn < Integer.MAX_VALUE;
				currentColumn++;
			}
			currentByte = x;
		}
		return currentByte;
	}

	private static int readChar () {
		int x = read();
		if (x < 0x20) {
			signalInputError(INVALID_CHARACTER);
		} else if (x > 0x7f) {
			signalInputError(UNSUPPORTED_CHARACTER);
		}
		return x;
	}

	private static String readString () {
		StringBuilder b = new StringBuilder();
		int x = readChar();
		while (x != '"') {
			if (x == '\\') {
				x = readChar();
				if (x == '"') {
					b.append('"');
				} else if (x == '\\') {
					b.append('\\');
				} else if (x == '/') {
					b.append('/');
				} else if (x == 'b') {
					b.append('\b');
				} else if (x == 'f') {
					b.append('\f');
				} else if (x == 'n') {
					b.append('\n');
				} else if (x == 'r') {
					b.append('\r');
				} else if (x == 't') {
					b.append('\t');
				} else if (x == 'u') {
					signalInputError(UNSUPPORTED_ESCAPE_SEQUENCE);
				} else {
					signalInputError(INVALID_ESCAPE_SEQUENCE);
				}
			} else {
				b.append((char) x);
			}
			x = readChar();
		}
		return b.toString();
	}

	private static int skipWhitespace () {
		int x = read();
		while ((x == ' ') || (x == '\t') || (x == '\n') || (x == '\r')) {
			x = read();
		}
		return x;
	}

	public static void main (String[] args) {
		if ((args.length < 2) || !args[0].equals("make")) {
			exit(USAGE);
		}

		File cdbFile = null;
		try {
			cdbFile = new File(args[1]).getCanonicalFile();
		} catch (IOException e) {
			exit(CREATE_CDB_FILE_ERROR);
		}

		File tempFile = null;
		try {
			tempFile = File.createTempFile("cdb", null, cdbFile.getParentFile());
		} catch (IOException e) {
			exit(CREATE_CDB_FILE_ERROR);
		}

		tempFile.deleteOnExit();

		try {
			cdb = new RandomAccessFile(tempFile, "rw");
		} catch (IOException e) {
			exit(CREATE_CDB_FILE_ERROR);
		}

		try {
			cdbWriter = new CdbWriter(cdb);
		} catch (IOException e) {
			closeAndExit(cdb, WRITE_CDB_FILE_ERROR);
		}

		currentByte = '\n';
		currentLine = 0;
		currentColumn = 0;

		int x = skipWhitespace();
		if (x == '{') {
			x = skipWhitespace();
			if ((x != '"') && (x != '}')) {
				signalInputError(STRING_OR_END_OF_OBJECT_EXPECTED);
			}
			while (x == '"') {
				String domain = readString();
				x = skipWhitespace();
				if (x != ':') {
					signalInputError(COLON_EXPECTED);
				}
				ArrayList<String> keys = null;
				x = skipWhitespace();
				if (x == 'n') {
					x = read();
					if (x != 'u') {
						signalInputError(NULL_EXPECTED);
					}
					x = read();
					if (x != 'l') {
						signalInputError(NULL_EXPECTED);
					}
					x = read();
					if (x != 'l') {
						signalInputError(NULL_EXPECTED);
					}
				} else if (x == '[') {
					x = skipWhitespace();
					if ((x != '"') && (x != ']')) {
						signalInputError(STRING_OR_END_OF_ARRAY_EXPECTED);
					}
					keys = new ArrayList<String>();
					while (x == '"') {
						keys.add(readString());
						x = skipWhitespace();
						if ((x != ',') && (x != ']')) {
							signalInputError(COMMA_OR_END_OF_ARRAY_EXPECTED);
						}
						if (x == ',') {
							x = skipWhitespace();
							if (x != '"') {
								signalInputError(STRING_EXPECTED);
							}
						}
					}
					if (x != ']') {
						signalInputError(END_OF_ARRAY_EXPECTED);
					}
				} else {
					signalInputError(NULL_OR_ARRAY_EXPECTED);
				}
				emit(domain, keys);
				x = skipWhitespace();
				if ((x != ',') && (x != '}')) {
					signalInputError(COMMA_OR_END_OF_OBJECT_EXPECTED);
				}
				if (x == ',') {
					x = skipWhitespace();
					if (x != '"') {
						signalInputError(STRING_EXPECTED);
					}
				}
			}
			if (x != '}') {
				signalInputError(END_OF_OBJECT_EXPECTED);
			}
			x = skipWhitespace();
			if (x != -1) {
				signalInputError(END_OF_TEXT_EXPECTED);
			}
		} else {
			signalInputError(OBJECT_EXPECTED);
		}

		close(cdbWriter);

		boolean r = tempFile.renameTo(cdbFile);
		if (!r) {
			cdbFile.delete();
			r = tempFile.renameTo(cdbFile);
		}
		if (!r) {
			exit(WRITE_CDB_FILE_ERROR);
		}

		System.exit(0);
	}
}