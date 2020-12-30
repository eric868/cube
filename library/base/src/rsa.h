/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: rsa.h
Version: 1.0
Date: 2016.1.13

History:
base     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __VBASE_RSA_H__
#define __VBASE_RSA_H__

#include <string>
#include "base.h"
namespace base
{

const int rsa_exponent = 17;
class COMMON_API rsa
{
public:
	rsa();
	~rsa();

public:
	/*
	 * initialize rsa (auto generate key pairs)
	 * @param key_length can be 256, 512, 1024.
	 * @return true if initialize successfully, false if failed
	 */
	bool init_key(int key_length = 1024);
	/*
	 * encrypt text
	 * @param plain[in] the plain text that need to encrypt
	 * @param cipher[out] output encrypted data
	 * @return true if initialize successfully, false if failed
	 */
	bool encrypt(const char* plain, std::string& cipher);
	/*
	 * decrypt text
	 * @param cipher[in] the cipher text that need to decrypted
	 * @param plain[out] output descrypted data
	 * @return true if initialize successfully, false if failed
	 */
	bool decrypt(const char* cipher, std::string& plain);
	/*
	 * export current public key as string
	 * @param public_key[out] target public key
	 * @return true if initialize successfully, false if failed
	 */
	bool export_public_key(std::string& public_key);
	/*
	 * export current private key as string
	 * @param private_key[out] target private key
	 * @return true if initialize successfully, false if failed
	 */
	bool export_private_key(std::string& private_key);
	/*
	 * import public key
	 * once import successfully, rsa can only be used to encrypt message
	 * @param public_key
	 * @return true if initialize successfully, false if failed
	 */
	bool import_public_key(const std::string& public_key);
	/*
	 * import private key
	 * once import successfully, rsa can only be used to decrypt message
	 * @param private_key
	 */
	bool import_private_key(const std::string& private_key);

private:
	void* key_;
	int key_length_;
};

}

#endif
