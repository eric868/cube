/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: base64.h
Version: 1.0
Date: 2016.1.13

History:
base     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __VBASE_BASE64_H__
#define __VBASE_BASE64_H__

#include <string>

namespace base
{

/*
 * base64 alphabet table, a mapping between integers and base64 digits
 * notes: This is an extern here but is defined in Base64.c
 */
extern const char b64_alphabet[];

/*
 * encode a string of characters as base64
 * @param output the output buffer for the encoding, stores the encoded string
 * @param input the input buffer for the encoding, stores the binary to be encoded
 * @param input_length the length of the input buffer, in bytes
 * @return the length of the encoded string
 */
int base64_encode(char* output, const char* input, int input_length = -1);

/*
 * encode a string of characters as base64
 * @param input the input buffer for the encoding, stores the binary to be encoded
 * @param input_length the length of the input buffer, in bytes
 * @return the encoded string
 */
std::string base64_encode(const char* input, int input_length = -1);

/*
 * decode a base64 encoded string into bytes
 * @param output the output buffer for the decoding, stores the decoded binary
 * @param input the input buffer for the decoding, stores the base64 string to be decoded
 * @param input_length the length of the input buffer, in bytes
 * @return the length of the decoded string
 */
int base64_decode(char* output, const char* input, int input_length = -1);

/*
 * decode a base64 encoded string into bytes
 * @param input the input buffer for the decoding, stores the base64 string to be decoded
 * @param input_length the length of the input buffer, in bytes
 * @return the decoded string
 */
std::string base64_decode(const char* input, int input_length = -1);

/* returns the length of a base64 encoded string whose decoded form is inputLen bytes long
 * @param input_length: the length of the decoded string
 * @return the length of a base64 encoded string whose decoded form is inputLen bytes long
 */
int base64_enc_len(int input_length);

/*
 * returns the length of the decoded form of a base64 encoded string
 * @param input the base64 encoded string to be measured
 * @param input_length the length of the base64 encoded string
 * @return the length of the decoded form of a base64 encoded string
 */
int base64_dec_len(const char* input, int input_length);

}

#endif // __VBASE_BASE64_H__
