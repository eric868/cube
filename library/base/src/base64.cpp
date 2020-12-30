/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: base64.cpp
Version: 1.0
Date: 2016.1.13

History:
base     2016.1.13   1.0     Create
******************************************************************************/

#include "base64.h"
#include <string.h>

namespace base
{

const char b64_alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

/* 'Private' declarations */
inline void a3_to_a4(unsigned char * a4, unsigned char * a3);
inline void a4_to_a3(unsigned char * a3, unsigned char * a4);
inline unsigned char b64_lookup(char c);

int base64_encode(char* output, const char* input, int input_length)
{
	if (input_length < 0) {
		input_length = strlen(input);
	}

	int i = 0, j = 0;
	int enc_length = 0;
	unsigned char a3[3];
	unsigned char a4[4];

	while (input_length--) {
		a3[i++] = *(input++);
		if (i == 3) {
			a3_to_a4(a4, a3);

			for (i = 0; i < 4; i++) {
				output[enc_length++] = b64_alphabet[a4[i]];
			}

			i = 0;
		}
	}

	if (i) {
		for (j = i; j < 3; j++) {
			a3[j] = '\0';
		}

		a3_to_a4(a4, a3);

		for (j = 0; j < i + 1; j++) {
			output[enc_length++] = b64_alphabet[a4[j]];
		}

		while ((i++ < 3)) {
			output[enc_length++] = '=';
		}
	}
	output[enc_length] = '\0';
	return enc_length;
}

std::string base64_encode(const char* input, int input_length)
{
	if (input_length < 0) {
		input_length = strlen(input);
	}

	int enc_length = base64_enc_len(input_length);
	char* enc_string = new char[enc_length + 1];
	base64_encode(enc_string, input, input_length);

	std::string ret = enc_string;
	delete[]enc_string;

	return ret;
}

int base64_decode(char* output, const char* input, int input_length)
{
	if (input_length < 0) {
		input_length = strlen(input);
	}

	int i = 0, j = 0;
	int dec_length = 0;
	unsigned char a3[3];
	unsigned char a4[4];

	while (input_length--) {
		if (*input == '=') {
			break;
		}

		a4[i++] = *(input++);
		if (i == 4) {
			for (i = 0; i < 4; i++) {
				a4[i] = b64_lookup(a4[i]);
			}

			a4_to_a3(a3, a4);

			for (i = 0; i < 3; i++) {
				output[dec_length++] = a3[i];
			}
			i = 0;
		}
	}

	if (i) {
		for (j = i; j < 4; j++) {
			a4[j] = '\0';
		}

		for (j = 0; j < 4; j++) {
			a4[j] = b64_lookup(a4[j]);
		}

		a4_to_a3(a3, a4);

		for (j = 0; j < i - 1; j++) {
			output[dec_length++] = a3[j];
		}
	}
	output[dec_length] = '\0';
	return dec_length;
}

std::string base64_decode(const char* input, int input_length)
{
	if (input_length < 0) {
		input_length = strlen(input);
	}

	int dec_length = base64_dec_len(input, input_length);
	char* dec_string = new char[dec_length + 1];
	base64_decode(dec_string, input, input_length);

	std::string ret;
	ret.append(dec_string, dec_length);
	delete[]dec_string;

	return ret;
}

int base64_enc_len(int input_length)
{
	int n = input_length;
	return (n + 2 - ((n + 2) % 3)) / 3 * 4;
}

int base64_dec_len(const char* input, int input_length)
{
	int i = 0;
	int num_eq = 0;
	for (i = input_length - 1; input[i] == '='; i--) {
		num_eq++;
	}
	return ((6 * input_length) / 8) - num_eq;
}

inline void a3_to_a4(unsigned char* a4, unsigned char* a3)
{
	a4[0] = (a3[0] & 0xfc) >> 2;
	a4[1] = ((a3[0] & 0x03) << 4) + ((a3[1] & 0xf0) >> 4);
	a4[2] = ((a3[1] & 0x0f) << 2) + ((a3[2] & 0xc0) >> 6);
	a4[3] = (a3[2] & 0x3f);
}

inline void a4_to_a3(unsigned char* a3, unsigned char* a4)
{
	a3[0] = (a4[0] << 2) + ((a4[1] & 0x30) >> 4);
	a3[1] = ((a4[1] & 0xf) << 4) + ((a4[2] & 0x3c) >> 2);
	a3[2] = ((a4[2] & 0x3) << 6) + a4[3];
}

inline unsigned char b64_lookup(char c)
{
	if (c >= 'A' && c <= 'Z') return c - 'A';
	if (c >= 'a' && c <= 'z') return c - 71;
	if (c >= '0' && c <= '9') return c + 4;
	if (c == '+') return 62;
	if (c == '/') return 63;
	return -1;
}

}
