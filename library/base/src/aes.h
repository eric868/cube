/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: aes.h
Version: 1.0
Date: 2016.1.13

History:
base     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __VBASE_AES_H__
#define __VBASE_AES_H__

#include <string>
#include "ssl/include/openssl/aes.h"
#include "base.h"

namespace base
{

class COMMON_API aes
{
public:
	aes();
	~aes();

public:
	/*
	 * set encrypt user key
	 * if the key is not set, aes will use default encrypt key
	 * @param key[in] encrypt key string, should be 16, 24 or 32 bytes
	 * @param bits[in] bits of the key, can be 128, 192 or 256
	 * @return 0 if successful, -1 if key is null, -2 if bits of the key is not in [128, 192, 256]
	 */
	int set_encrypt_key(const char* key, const int bits);
	/*
	 * set decrypt user key
	 * it the key is not set, aes will use default decrypt key
	 * @param key[in] decrypt key string, should be 16, 24 or 32 bytes
 	 * @param bits[in] bits of the key, can be 128, 192 or 256
 	 * @return 0 if successful, -1 if key is null, -2 if bits of the key is not in [128, 192, 256]
 	 */
	int set_decrypt_key(const char* key, const int bits);
	/*
	 * encrypt input text
	 * @param out[out] used to store output cipher text
	 * @param in[in] input plain text which to be encoded
	 * @param length[in] length of the input plain text, if -1, it will calculate the length automatically
	 * @return length of the output cipher text, if -1, encrypt failed
	 */
	int encrypt(char* out, const char* in, int length = -1);
	/*
	 * encrypt input text and encode as base64 string
	 * @param in[in] input plain text which to be encoded
	 * @param length[in] length of the input plain text, if -1, it will calculate the length automatically
	 * @return output cipher text, if empty, encrypt failed
	 */
	std::string encrypt_as_base64(const char* in, int length = -1);
	/*
	 * decrypt input text
	 * @param out[out] used to store output plain text
	 * @param in[in] input cipher text which to be decoded
	 * @param length[in] length of the input cipher text, must be specified
	 * @return length of the output plain text, if -1, decrypt failed
	 */
	int decrypt(char* out, const char* in, int length);
	/*
	 * decode input base64 string and decrypt it
	 * @param in[in] input cipher text which to be decoded
	 * @param length[in] length of the input cipher text, if -1, it will calculate the length automatically
	 * @return output plain text, if empty, decrypt failed
	 */
	std::string decrypt_base64(const char* in, int length = -1);

private:
	AES_KEY encrypt_key_;
	AES_KEY decrypt_key_;
};

}

#endif // __VBASE_AES_H__
