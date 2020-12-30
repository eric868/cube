/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: file.h
Version: 1.0
Date: 2016.1.13

History:
base     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __BASE_FILE_H__
#define __BASE_FILE_H__

#if defined(_MSC_VER)
#include <Windows.h>
#else
#endif
#include <string>
#include "base.h"
namespace base {

enum FILE_FLAG
{
	FILE_FLAG_READ = 1,
	FILE_FLAG_WRITE = 2,
};

enum FILE_SEEK_TYPE
{
	FILE_SEEK_CUR = 1,
	FILE_SEEK_END = 2,
	FILE_SEEK_SET = 0,
};

#ifndef byte
typedef unsigned char byte;
#endif

class COMMON_API file
{
public:
	file();
	~file();

public:
	int open(const char* file_path, int flags = FILE_FLAG_READ | FILE_FLAG_WRITE);
	int close();
	size_t size();
	int seek(int pos, FILE_SEEK_TYPE type);
	int read(byte* buffer, int size);
	int write(const byte* buffer, int size);

	bool is_open() const;

public:
	static bool is_file_exist(const char* file_path);
	static bool is_directory_exist(const char* file_path);
	static bool rename_director(const char* src_path, const char* des_path);
	static bool rename_file(const char* src_file_path, const char* des_file_path);
	static bool remove_file(const char* file_path);

public:
	static bool make_sure_directory_exist(const char* dir_path);
#ifndef _MSC_VER
	static int make_dirs(const char* dir_path);
#endif
    static std::string file_directory(const char* file_path);

private:
	int read_all(void const* buffer, int size);
	int write_all(void const* buffer, int size);

private:
#if defined(_MSC_VER)
	HANDLE file_;
#else
	int file_;
#endif
};

} // end of namespace base

#endif
