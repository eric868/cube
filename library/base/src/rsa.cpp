/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: rsa.cpp
Version: 1.0
Date: 2016.1.13

History:
base     2016.1.13   1.0     Create
******************************************************************************/

#include "rsa.h"
#include "base.h"
#include <string.h>
#include <ssl/include/openssl/rsa.h>
#include <ssl/include/openssl/rand.h>

namespace base
{

const char rsa_seed[] = "bring the all happiness to you~";

int htoi2(const char* text, int len)
{
	int result = 0;
	char ch;
	int val;
	int ev = 1;

	for (int i = len - 1; i >= 0; i--)
	{
		val = 0;
		ch = text[i];

		if (ch >= '0' && ch <= '9') {
			val = ch - '0';
		}

		if (ch >= 'A' && ch <= 'F') {
			val = ch - 'A' + 0x0A;
		}

		if (ch >= 'a' && ch <= 'f') {
			val = ch - 'a' + 0x0A;
		}

		result += val * ev;
		ev *= 16;
	}
	return result;
}

rsa::rsa()
	: key_(NULL)
	, key_length_(1024)
{
}

rsa::~rsa()
{
	if (key_ != NULL) {
		RSA_free((RSA*)key_);
		key_ = NULL;
	}
}

bool rsa::init_key(int key_length)
{
	if (key_length < 128 || key_length % 128 != 0) {
		return false;
	}

	if (key_ != NULL) {
		RSA_free((RSA*)key_);
		key_ = NULL;
	}

	RAND_seed(rsa_seed, sizeof(rsa_seed));
	key_length_ = key_length;
	key_ = (void*)RSA_generate_key(key_length, rsa_exponent, NULL, NULL);

	return true;
}

bool rsa::encrypt(const char* plain, std::string& cipher)
{
	if (key_ == NULL) {
		return false;
	}

	int rsa_max_pack_len = key_length_ / 8 - 11;

	char tmp[1024 * 2];
	char out[1024 * 2];
	memset(tmp, 0, sizeof(tmp));
	memset(out, 0, sizeof(out));

	cipher.clear();
	int plain_len = strlen(plain);

	int num = RSA_public_encrypt(plain_len, (const unsigned char *)plain,
			(unsigned char*)tmp, ((RSA*)key_), RSA_PKCS1_PADDING);
	if (num == -1 && plain_len > rsa_max_pack_len)
	{
		char str[1024 * 2];
		int pos = 0;
		while (pos < plain_len) {
			int copy_len = BMIN(rsa_max_pack_len, plain_len - pos);
			memcpy(str, plain + pos, copy_len);
			num = RSA_public_encrypt(copy_len, (unsigned char*)str,
					(unsigned char*)tmp, ((RSA*)key_), RSA_PKCS1_PADDING);
			for (int i = 0; i < num; i++) {
				sprintf(out + i * 2, "%02x", (unsigned char)tmp[i]);
			}
			pos += copy_len;
			cipher += out;
		}
	}
	else if (num > 0) {
		for (int i = 0; i < num; i++) {
			sprintf(out + strlen(out), "%02x", (unsigned char)tmp[i]);
		}
		cipher = out;
	}
	else {
		return false;
	}
	return true;
}

bool rsa::decrypt(const char* cipher, std::string& plain)
{
	if (key_ == NULL) {
		return false;
	}

	int num = strlen(cipher);
	if (num % 2 != 0) {
		return false;
	}

	char tmp[1024 * 2];
	char src[1024 * 2];
	char out[1024 * 2];

	plain.clear();
	memset(tmp, 0, sizeof(tmp));
	for (int i = 0; i < num; i++) {
		tmp[i] = htoi2(cipher + i * 2, 2);
	}

	int cipher_segment_len = key_length_ / 4;
	int cipher_len = strlen(cipher);
	num = num / 2;
	num = RSA_private_decrypt(num, (unsigned char*)tmp,
			(unsigned char*)out, ((RSA*)key_), RSA_PKCS1_PADDING);
	if (num <= 0 && cipher_len > cipher_segment_len)
	{
		if (cipher_len % cipher_segment_len != 0) {
			return false;
		}

		for (int k = 0; k < cipher_len / cipher_segment_len; k++)
		{
			memset(tmp, 0, sizeof(tmp));
			memset(out, 0, sizeof(out));

			memcpy(src, cipher + k * cipher_segment_len, cipher_segment_len);
			src[cipher_segment_len] = 0;

			num = cipher_segment_len;
			for (int i = 0; i < num; i++) {
				tmp[i] = htoi2(src + i * 2, 2);
			}

			num = num / 2;
			num = RSA_private_decrypt(num, (unsigned char*)tmp,
					(unsigned char*)(out + strlen(out)), ((RSA*)key_), RSA_PKCS1_PADDING);
			plain += out;
		}
	}
	else if (num > 0) {
		plain = out;
	}
	else {
		return false;
	}
    plain = plain.substr(0, num);
	return true;
}


bool rsa::export_public_key(std::string& public_key)
{
	if (key_ == NULL) {
		return false;
	}

	if (((RSA*)key_)->n == 0) {
		return false;
	}

	char tmp[2048];
	unsigned char str[2048];
	int len = BN_bn2bin(((RSA*)key_)->n, str);
	for (int i = 0; i < len; i++) {
		sprintf(tmp + i * 2, "%02x", str[i]);
	}

	public_key = tmp;
	return true;
}

bool rsa::export_private_key(std::string& private_key)
{
	if (key_ == NULL) {
		return false;
	}

	if (((RSA*)key_)->n == 0) {
		return false;
	}

	char tmp[2048];
	unsigned char str[2048];
	int len = BN_bn2bin(((RSA*)key_)->d, str);
	for (int i = 0; i < len; i++) {
		sprintf(tmp + i * 2, "%02x", str[i]);
	}

	private_key = tmp;
	return true;
}

bool rsa::import_public_key(const std::string& public_key)
{
	if (public_key.length() % 2 != 0) {
		return false;
	}

	if (key_ != NULL) {
		RSA_free((RSA*)key_);
		key_ = NULL;
	}

	RAND_seed(rsa_seed, sizeof(rsa_seed));
	key_ = (void*)RSA_new();
	key_length_ = public_key.length() * 4;

	unsigned char cpublic_key[256];
	int num = public_key.length() / 2;
	for (int i = 0; i < num; i++) {
		cpublic_key[i] = htoi2(public_key.c_str() + i * 2, 2);
	}
	((RSA*)key_)->n = BN_bin2bn((unsigned char*)cpublic_key, num, ((RSA*)key_)->n);

	char cexponent[10], tmp[10];
	sprintf(tmp, "%02x", rsa_exponent);
	cexponent[0] = htoi2(tmp, 2);
	((RSA*)key_)->e = BN_bin2bn((unsigned char*)cexponent, 1, ((RSA*)key_)->e);

	return true;
}

bool rsa::import_private_key(const std::string& private_key)
{
	if (private_key.length() % 2 != 0) {
		return false;
	}

	if (key_ != NULL) {
		RSA_free((RSA*)key_);
		key_ = NULL;
	}

	RAND_seed(rsa_seed, sizeof(rsa_seed));
	key_ = (void*)RSA_new();
	key_length_ = private_key.length() * 4;

	unsigned char cprivate_key[256];
	int num = private_key.length() / 2;
	for (int i = 0; i < num; i++) {
		cprivate_key[i] = htoi2(private_key.c_str() + i * 2, 2);
	}
	((RSA*)key_)->d = BN_bin2bn((unsigned char*)cprivate_key, num, ((RSA*)key_)->d);

	char cexponent[10], tmp[10];
	sprintf(tmp, "%02x", rsa_exponent);
	cexponent[0] = htoi2(tmp, 2);
	((RSA*)key_)->e = BN_bin2bn((unsigned char*)cexponent, 1, ((RSA*)key_)->e);

	return true;
}

}
