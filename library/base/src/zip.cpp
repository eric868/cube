/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: zip.cpp
Version: 1.0
Date: 2016.1.13

History:
base     2016.1.13   1.0     Create
******************************************************************************/

#include "zip.h"
#include "trace.h"

namespace base {

zip::zip()
{
	init();
}

zip::~zip()
{
    /* clean up */
    (void)deflateEnd(&defstrm_);
    (void)inflateEnd(&infstrm_);
}

int zip::init(int level)
{
	if(level == Z_DEFAULT_COMPRESSION) {
		/* allocate deflate state */
		defstrm_.zalloc = Z_NULL;
		defstrm_.zfree = Z_NULL;
		defstrm_.opaque = Z_NULL;
		int ret = deflateInit(&defstrm_, level);
		if(ret != Z_OK) {
			zerr(ret);
			return ret;
		}

		/* allocate inflate state */
		infstrm_.zalloc = Z_NULL;
		infstrm_.zfree = Z_NULL;
		infstrm_.opaque = Z_NULL;
		infstrm_.avail_in = 0;
		infstrm_.next_in = Z_NULL;

		ret = inflateInit(&infstrm_);
		if (ret != Z_OK) {
			zerr(ret);
			return ret;
		}
	}
	else {
		int ret = deflateInit(&defstrm_, level);
		if(ret != Z_OK) {
			zerr(ret);
			return ret;
		}
	}
	return 0;
}

int zip::deflate_data(char *dest, long *dest_len, const char *source, long source_len)
{
	z_const Bytef * src = (z_const Bytef *)source;
	int ret;
	defstrm_.avail_in = source_len;
	defstrm_.next_in = src;
	defstrm_.avail_out = *dest_len;
	defstrm_.next_out = (Bytef *)dest;
    ret = deflate(&defstrm_, Z_FINISH);
	*dest_len -= defstrm_.avail_out;
	return ret;
}

int zip::inflate_data(char *dest, long *dest_len, const char *source, long source_len)
{
	z_const Bytef * src = (z_const Bytef *)source;
	int ret;
	infstrm_.avail_in = source_len;
	infstrm_.next_in = src;
	infstrm_.avail_out = *dest_len;
	infstrm_.next_out = (Bytef*)dest;
    ret = inflate(&infstrm_, Z_FINISH);
	*dest_len -= infstrm_.avail_out;
	return ret;
}

int zip::zcompress(char *dest, long *dest_len, const char *source, long source_len)
{
	return compress((Bytef*)dest, (uLongf*)dest_len, (const Bytef*)source, source_len);
}

int zip::unzcompress(char *dest, long *dest_len, const char *source, long source_len)
{
	return uncompress((Bytef*)dest, (uLongf*)dest_len, (const Bytef*)source, source_len);
}

int zip::zcompress(FILE *source, FILE *dest, int level)
{
    int ret, flush;
    unsigned have;
    z_stream strm;
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];

    /* allocate deflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = deflateInit(&strm, level);
    if (ret != Z_OK) {
        return ret;
    }

    /* compress until end of file */
    do {
        strm.avail_in = fread(in, 1, CHUNK, source);
        if (ferror(source)) {
            (void)deflateEnd(&strm);
            zerr(Z_ERRNO);
            return Z_ERRNO;
        }
        flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = in;

        /* run deflate() on input until output buffer not full, finish
        compression if all of source has been read in */
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = deflate(&strm, flush);    /* no bad return value */
            if (ret == Z_STREAM_ERROR) {
                zerr(ret);
                return ret;
            }
            have = CHUNK - strm.avail_out;
            if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
                (void)deflateEnd(&strm);
                zerr(Z_ERRNO);
                return Z_ERRNO;
            }
        } while (strm.avail_out == 0);

        if (strm.avail_in != 0) {
            zerr(Z_ERRNO);
            return Z_ERRNO;
        }

        /* done when last data in file processed */
    } while (flush != Z_FINISH);

    if (ret != Z_STREAM_END) {
        zerr(ret);
        return ret;
    }

    /* clean up and return */
    (void)deflateEnd(&strm);
    return Z_OK;
}


int zip::unzcompress(FILE *source, FILE *dest)
{
    int ret;
    unsigned have;
    z_stream strm;
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit(&strm);
    if (ret != Z_OK) {
        return ret;
    }

    /* decompress until deflate stream ends or end of file */
    do {
        strm.avail_in = fread(in, 1, CHUNK, source);
        if (ferror(source)) {
            (void)inflateEnd(&strm);
            zerr(Z_ERRNO);
            return Z_ERRNO;
        }
        if (strm.avail_in == 0) {
            break;
        }
        strm.next_in = in;

        /* run inflate() on input until output buffer not full */
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = inflate(&strm, Z_NO_FLUSH);
            if (ret == Z_STREAM_ERROR) {
            	zerr(ret);
            	return ret;
            }
            have = CHUNK - strm.avail_out;
            if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
                (void)inflateEnd(&strm);
                zerr(Z_ERRNO);
                return Z_ERRNO;
            }
        } while (strm.avail_out == 0);

        /* done when inflate() says it's done */
    } while (ret != Z_STREAM_END);

    /* clean up and return */
    (void)inflateEnd(&strm);
    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

void zip::zerr(int ret)
{
    switch (ret) {
	case Z_ERRNO:
		TRACE_INFO("base", "zlib io error~~~");
		break;
	case Z_STREAM_ERROR:
		TRACE_INFO("base", "zlib invalid compression level");
		break;
	case Z_NEED_DICT:
	case Z_DATA_ERROR:
		TRACE_INFO("base", "zlib invalid or incomplete deflate data");
		break;
	case Z_MEM_ERROR:
		TRACE_INFO("base", "zlib out of memory");
		break;
	case Z_VERSION_ERROR:
		TRACE_INFO("base", "zlib version mismatch!");
		break;
	default:
		TRACE_INFO("base", "zlib error~~~");
		break;
    }
}

}
