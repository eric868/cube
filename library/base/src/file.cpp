/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: file.cpp
Version: 1.0
Date: 2016.1.13

History:
base     2016.1.13   1.0     Create
******************************************************************************/

#include "file.h"

#if defined(_MSC_VER)
#include <iostream>
#include <imagehlp.h>
#pragma comment(lib, "imagehlp.lib")
#else
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#endif
#include <assert.h>

namespace base {

const int MAX_READ_BUFFER_SIZE = 16 * 1024 * 1024;

file::file()
	: file_(0)
{

}

file::~file()
{
	close();
}

int file::open(const char* file_path, int flags)
{
	std::string dir = file::file_directory(file_path);
	if (!file::make_sure_directory_exist(dir.c_str())) {
		return -1;
	}

#if defined(_MSC_VER)
	DWORD access = 0;
	if (flags & FILE_FLAG_READ) {
		access |= GENERIC_READ;
	}
	if (flags & FILE_FLAG_WRITE) {
		access |= GENERIC_WRITE;
	}

	DWORD attrs = 0;
	if (file::is_file_exist(file_path)) {
		attrs = OPEN_EXISTING;
	}
	else {
		attrs = CREATE_NEW;
	}
	file_ = ::CreateFile((LPCWSTR)file_path,
		access,
		FILE_SHARE_READ,
		NULL,
		attrs,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	return (file_ == INVALID_HANDLE_VALUE) ? -1 : 0;
#else
	int sys_flags = 0;
	if ((flags & FILE_FLAG_READ) && (flags & FILE_FLAG_WRITE)) {
		sys_flags |= (O_RDWR | O_CREAT);
	}
	else if (flags & FILE_FLAG_READ) {
		sys_flags |= (O_RDONLY | O_CREAT);
	}
	else if (flags & FILE_FLAG_WRITE) {
		sys_flags |= (O_WRONLY | O_CREAT);
	}

	file_ = ::open(file_path, sys_flags, 0755);
	return file_ > 0 ? 0 : -1;
#endif

	return -1;
}

int file::close()
{
	if (file_ > 0) {
#if defined(_MSC_VER)
		::CloseHandle(file_);
#else
		::close(file_);
#endif
	}

	file_ = 0;
	return 0;
}

size_t file::size()
{
	assert(file_ > 0);

#if defined(_MSC_VER)
	LARGE_INTEGER size;
	size.QuadPart = 0;
	if (::GetFileSizeEx(file_, &size)) {
		return (size_t)size.QuadPart;
	}
#else
	struct stat stat;
	if (fstat(file_, &stat) == 0) {
		return stat.st_size;
	}
#endif

	return 0;
}

int file::seek(int pos, FILE_SEEK_TYPE type)
{
	assert(file_ > 0);

#if defined(_MSC_VER)
	int seek_type;
	switch (type) {
	case FILE_SEEK_CUR:
		seek_type = FILE_CURRENT;
		break;
	case FILE_SEEK_END:
		seek_type = FILE_END;
		break;
	case FILE_SEEK_SET:
	default:
		seek_type = FILE_BEGIN;
		break;
	}

	LARGE_INTEGER in, out;
	in.QuadPart = pos;
	out.QuadPart = 0;
	if (::SetFilePointerEx(file_, in, &out, seek_type)) {
		return 0;
	}
#else
	int seek_type;
	switch (type) {
	case FILE_SEEK_CUR:
		seek_type = SEEK_CUR;
		break;
	case FILE_SEEK_END:
		seek_type = SEEK_END;
		break;
	case FILE_SEEK_SET:
	default:
		seek_type = SEEK_SET;
	}

	int ret = (int)lseek(file_, pos, seek_type);
	if (ret >= 0) {
		return 0;
	}
#endif

	return -1;
}

int file::read(byte* buffer, int size)
{
	assert(file_ > 0);

	return read_all(buffer, size);
}

int file::write(const byte* buffer, int size)
{
	assert(file_ > 0);

#if defined(_MSC_VER)
	DWORD write_size = 0;
	if (::WriteFile(file_, (void*)buffer, size, &write_size, NULL)) {
		return (int)write_size;
	}
#else
	int write_size = write_all((void*)buffer, size);
	return write_size;
#endif

	return -1;
}

int file::read_all(void const* buffer, int size)
{
#if defined(_MSC_VER)
	char* dst = (char*)buffer;
	int total_read_size = 0;
	while (size != 0) {
		int quant = size > MAX_READ_BUFFER_SIZE ? MAX_READ_BUFFER_SIZE : size;
		DWORD read_size;
		if (!::ReadFile(file_, (void*)dst, size, &read_size, NULL)) {
			return -1;
		}
		else {
			dst += (int)read_size;
			total_read_size += (int)read_size;
			size -= (int)read_size;
		}
	}
	return total_read_size;
#else
	char* dst = (char*)buffer;
	int total_read_size = 0;
	while (size != 0) {
		int quant = size > MAX_READ_BUFFER_SIZE ? MAX_READ_BUFFER_SIZE : size;
		int read_size = (int)::read(file_, dst, quant);
		if (read_size < 0) {
			return -1;
		}
        else if (read_size == 0) {
            break;
        }
		else {
			dst += read_size;
			total_read_size += read_size;
			size -= read_size;
		}
	}
	return total_read_size;
#endif
}

int file::write_all(void const* buffer, int size)
{
#if defined(_MSC_VER)
#else
	char* src = (char*)buffer;
	int rc;
	int written = 0;
	while (size > 0 && (rc = (int)::write(file_, src, size)) > 0) {
		written += rc;
		size -= rc;
		src += rc;
	}
	return written;
#endif
	return -1;
}

bool file::is_open() const
{
#if defined(_MSC_VER)
	return (file_ != INVALID_HANDLE_VALUE && file_);
#else
	return file_ > 0;
#endif
	return false;
}

bool file::is_file_exist(const char* file_path)
{
#if defined(_MSC_VER)
	DWORD attr = GetFileAttributes((LPCWSTR)file_path);
	if (attr != INVALID_FILE_ATTRIBUTES &&
		attr != FILE_ATTRIBUTE_DIRECTORY) {
		return true;
	}
#else
	struct stat stat;
	if (lstat(file_path, &stat) == 0) {
		return S_ISREG(stat.st_mode);
	}
#endif

	return false;
}

bool file::is_directory_exist(const char* file_path)
{
#if defined(_MSC_VER)
	DWORD attr = GetFileAttributes((LPCWSTR)file_path);
	if (attr != INVALID_FILE_ATTRIBUTES &&
		attr == FILE_ATTRIBUTE_DIRECTORY) {
		return true;
	}
#else
	struct stat stat;
	if (lstat(file_path, &stat) == 0) {
		return S_ISDIR(stat.st_mode);
	}
#endif

	return false;
}
    
bool file::rename_director(const char* src_path, const char* des_path)
{
    if (!is_directory_exist(src_path)) {
        return false;
    }
    return (rename(src_path, des_path) == 0);
}

bool file::rename_file(const char* src_file_path, const char* des_file_path)
{
	return (rename(src_file_path, des_file_path) == 0);
}

bool file::remove_file(const char* file_path)
{
	return (remove(file_path) == 0);
}

bool file::make_sure_directory_exist(const char* dir_path)
{
	if (file::is_directory_exist(dir_path)) {
		return true;
	}
#if defined(_MSC_VER)
	return (MakeSureDirectoryPathExists(dir_path) == TRUE);
#else
    if (file::make_dirs(dir_path) == 0) {
        return true;
    }
#endif

	return false;
}
    
#ifndef _MSC_VER
int file::make_dirs(const char* dir_path)
{
    char *saved_path, *cp;
    int saved_ch;
    struct stat st;
    int rc;
    
    cp = saved_path = strdup(dir_path);
    while (*cp && *cp == '/') {
        ++cp;
    }
    while (1)
    {
        while (*cp && *cp != '/') ++cp;
        if ((saved_ch = *cp) != 0) {
            *cp = 0;
        }
        if ((rc = stat(saved_path, &st)) >= 0) {
            if (!S_ISDIR(st.st_mode)) {
                errno = ENOTDIR;
                rc = -1;
                break;
            }
        }
        else {
            if (errno != ENOENT ) {
                break;
            }
            
            if ((rc = mkdir(saved_path, 0755)) < 0 ) {
                if (errno != EEXIST)
                    break;
                
                if ((rc = stat(saved_path, &st)) < 0)
                    break;
                
                if (!S_ISDIR(st.st_mode)) {
                    errno = ENOTDIR;
                    rc = -1;
                    break;
                }
            }
        }
        
        if (saved_ch != 0) {
            *cp = saved_ch;
        }
        
        while (*cp && *cp == '/') {
            ++cp;
        }
        if (*cp == 0) {
            break;
        }
    }
    
    free(saved_path);
    return rc;
}
#endif

std::string file::file_directory(const char* file_path)
{
	std::string dir(file_path);
	int pos = (int)dir.rfind('/');
	if (pos >= 0) {
		return dir.substr(0, pos + 1);
	}
	else {
#if defined(_MSC_VER)
		pos = dir.rfind('\\');
		if (pos >= 0) {
			return dir.substr(0, pos + 1);
		}
#endif
	}

	return "";
}

} // end of namespace base
