/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: aes.cpp
Version: 1.0
Date: 2016.1.13

History:
base     2016.1.13   1.0     Create
******************************************************************************/

#include "aes.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "base64.h"
#include "trace.h"

const char* AES_DEFAULT_KEY = "0123456789ABCDEF";

namespace base
{

aes::aes()
{
	set_encrypt_key(AES_DEFAULT_KEY, 128);
	set_decrypt_key(AES_DEFAULT_KEY, 128);
}

aes::~aes()
{

}

int aes::set_encrypt_key(const char* key, const int bits)
{
	return AES_set_encrypt_key((const unsigned char*)key, bits, &encrypt_key_);
}

int aes::set_decrypt_key(const char* key, const int bits)
{
	return AES_set_decrypt_key((const unsigned char*)key, bits, &decrypt_key_);
}

int aes::encrypt(char* out, const char* in, int length)
{
	assert(out != NULL);
	assert(in != NULL);

	if (length == -1) {
		length = strlen((const char*)in);
	}

	unsigned char iv[AES_BLOCK_SIZE];
	memset(iv, 0, sizeof(iv));

	AES_cbc_encrypt((unsigned char*)in, (unsigned char*)out,
			length, &encrypt_key_,  iv, AES_ENCRYPT);

	return ((length + AES_BLOCK_SIZE - 1) / AES_BLOCK_SIZE) * AES_BLOCK_SIZE;
}

std::string aes::encrypt_as_base64(const char* in, int length)
{
	assert(in != NULL);

	char* out = new char[(length + AES_BLOCK_SIZE) * 2];
	int out_length = encrypt(out, in, length);

	std::string ret;
	if (out_length > 0) {
		ret = base::base64_encode(out, out_length);
	}
	delete[]out;
	return ret;
}

int aes::decrypt(char* out, const char* in, int length)
{
	assert(out != NULL);
	assert(in != NULL);

	if (length % AES_BLOCK_SIZE != 0) {
		TRACE_ERROR("base", 0, "length of the aes cipher must be a multiple of 16");
		return -1;
	}

	unsigned char iv[AES_BLOCK_SIZE];
	memset(iv, 0, sizeof(iv));

	AES_cbc_encrypt((unsigned char*)in, (unsigned char*)out,
			length, &decrypt_key_, iv, AES_DECRYPT);

	return length;
}

std::string aes::decrypt_base64(const char* in, int length)
{
	assert(in != NULL);

	if (length == -1) {
		length = strlen(in);
	}

	std::string in_plain = base::base64_decode(in, length);

	std::string ret;
	char* out = new char[in_plain.length() * 2];
	if (decrypt(out, in_plain.c_str(), in_plain.length()) > 0) {
		ret.append(out, in_plain.length());
	}
	delete[]out;
	return ret;
}

}
