/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: zip.h
Version: 1.0
Date: 2016.1.13

History:
base     2016.1.13   1.0     Create
******************************************************************************/

#include <stdio.h>
#include "zlib-1.2.8/include/zlib.h"
#include "base.h"

#ifndef __BASE_ZIP_H__
#define __BASE_ZIP_H__

namespace base {

#define CHUNK 262144 // 256k buff

class COMMON_API zip
{
public:
	zip();
	~zip();

public:
	/*
	 * init stream
	 * @blksize is the sizeof in or out data
	 * @level is the compress level
	 * return Z_OK(0) on success
	 */
	int init(int level = Z_DEFAULT_COMPRESSION);

	int deflate_data(char *dest, long *destlen, const char *source, long source_len);

	int inflate_data(char *dest, long *destlen, const char *source, long source_len);


public:
	/*
	 * Compress from file source to file dest until EOF on source.
	 * def() returns Z_OK on success, Z_MEM_ERROR if memory could not be
	 * allocated for processing, Z_STREAM_ERROR if an invalid compression
	 * level is supplied, Z_VERSION_ERROR if the version of zlib.h and the
	 * version of the library linked do not match, or Z_ERRNO if there is
	 * an error reading or writing the files.
	 */
	static int zcompress(FILE *source, FILE *dest, int level);
	/*
	 * The following utility functions are implemented on top of the basic
	 * stream-oriented functions.  To simplify the interface, some default options
	 * are assumed (compression level and memory usage, standard memory allocation
	 * functions).  The source code of these utility functions can be modified if
	 * you need special options.
	 */
	static int zcompress(char *dest, long *dest_len, const char *source, long source_len);
	/*
	 * Decompress from file source to file dest until stream ends or EOF.
	 * inf() returns Z_OK on success, Z_MEM_ERROR if memory could not be
	 * allocated for processing, Z_DATA_ERROR if the deflate data is
	 * invalid or incomplete, Z_VERSION_ERROR if the version of zlib.h and
	 * the version of the library linked do not match, or Z_ERRNO if there
	 * is an error reading or writing the files.
	 */
	static int unzcompress(FILE *source, FILE *dest);
	/*
	 * compressBound() returns an upper bound on the compressed size after
	 * compress() or compress2() on sourceLen bytes.  It would be used before a
	 * compress() or compress2() call to allocate the destination buffer.
	 */
	static int unzcompress(char *dest, long *dest_len, const char *source, long source_len);
	/*
	 * report a zlib or i/o error
	 */
	static void zerr(int ret);

private:
	z_stream infstrm_;
	z_stream defstrm_;
};

}
#endif
