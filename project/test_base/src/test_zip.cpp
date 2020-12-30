/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: test_zip.h
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#include "test_zip.h"
#include "base/zip.h"
#include <string.h>
#include <string>

#define MAX_NUM 1024*1024

#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#  include <fcntl.h>
#  include <io.h>
#  define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#else
#  define SET_BINARY_MODE(file)
#endif

using namespace base;
using namespace std;

test_zip::test_zip()
{
}

test_zip::~test_zip()
{
}

int test_zip::test_flate(int argc, char **argv)
{
	
	/* 原始数据 */
	unsigned char strSrc[] = "hello world! aaaaa bbbbb ccccc ddddd 中文测试 yes";
	unsigned char buf[1024] = { 0 };
	unsigned char strDst[1024] = { 0 };
	unsigned long srcLen = sizeof(strSrc);
	unsigned long bufLen = sizeof(buf);
	unsigned long dstLen = sizeof(strDst);
#if 1
    zip test_zip;
    test_zip.init(Z_BEST_COMPRESSION);
	printf("Src string:%s\nLength:%ld\n", strSrc, srcLen);

	/* 压缩 */
	test_zip.deflate_data((char *)buf, (long *)&bufLen, (char *)strSrc, srcLen);
	printf("After Compressed Length:%ld\n", bufLen);

	/* 解压缩 */
	test_zip.inflate_data((char *)strDst, (long *)&dstLen, (char *)buf, bufLen);
	printf("After UnCompressed Length:%ld\n", dstLen);

	printf("UnCompressed String:%s\n", strDst);
#endif
    printf("Src string:%s\nLength:%ld\n", strSrc, srcLen);

    /* 压缩 */
    base::zip::zcompress((char *)buf, (long *)&bufLen, (char *)strSrc, srcLen);
    printf("After Compressed Length:%ld\n", bufLen);

    /* 解压缩 */
    base::zip::unzcompress((char *)strDst, (long *)&dstLen, (char *)buf, bufLen);
    printf("After UnCompressed Length:%ld\n", dstLen);

    printf("UnCompressed String:%s\n", strDst);

	return 0;
}
