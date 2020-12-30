#ifndef FILE_H_
#define FILE_H_

#include "project.h"
#include <stdio.h>
#include <sys/stat.h>

#if defined(_MSC_VER)
#include <iostream>
#include <imagehlp.h>
#pragma comment(lib, "imagehlp.lib")
#else
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#endif

namespace eco{;
//"r" ：以只读方式打开文件，该文件必须存在。
//"w" ：打开只写文件，若文件存在则文件长度清为0，即该文件内容会消失。
//若文件不存在则建立该文件。
//"a" ：以附加的方式打开只写文件。若文件不存在，则会建立该文件，
//如果文件存在，写入的数据会被加到文件尾，即文件原先的内容会被保留。（EOF符保留）
//"r+" ：以可读写方式打开文件，该文件必须存在。
//"w+" ：打开可读写文件，若文件存在则文件长度清为零，即该文件内容会消失。
//若文件不存在则建立该文件。
//"a+"：以附加方式打开可读写的文件。若文件不存在，则会建立该文件，
//如果文件存在，写入的数据会被加到文件尾后，即文件原先的内容会被保留。（原来的EOF符不保留）
//"ab+"读写打开一个二进制文件，允许读，或在文件末追加数据。

enum FILE_SEEK_TYPE
{
    FILE_SEEK_CUR = 1,
    FILE_SEEK_END = 2,
    FILE_SEEK_SET = 0,
};

////////////////////////////////////////////////////////////////////////////////
class File : public eco::Object<File>
{
public:
    inline File() 
        : m_fp(nullptr)
        , path("")
    {}

    inline ~File()
    {
        close();
    }

    inline bool open(
        IN const char* file_path,
        IN const char* open_mode,
        IN eco::Error& e)
    {
        close();
        m_fp = ::fopen(file_path, open_mode);
        if (null())
        {
            e.id(-1) << "open file fail: " << file_path;
            return false;
        }
        path = file_path;
        return true;
    }

    inline void close()
    {
        if (m_fp != nullptr)
        {
            ::fclose(m_fp);
            m_fp = nullptr;
        }
    }

    inline size_t file_size()
    {
        struct stat stat;
        if (::stat(path.c_str(), &stat) == 0) {
            return stat.st_size;
        }
        return 0;
    }

    inline int seek(int pos, FILE_SEEK_TYPE type)
    {
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

        int ret = (int)fseek(m_fp, pos, seek_type);
        if (ret >= 0) {
            return 0;
        }

        return -1;
    }

    inline bool null() const
    {
        return m_fp == nullptr;
    }

    inline int read(IN char* buf, IN size_t size)
    {
        return ::fread(buf, 1, size, m_fp);
    }

    inline int write(IN const char* buf, IN size_t size)
    {
        return ::fwrite(buf, size, 1, m_fp);
    }

    inline void flush()
    {
        ::fflush(m_fp);
    }

    inline FILE* get()
    {
        return m_fp;
    }
    inline const FILE* get() const
    {
        return m_fp;
    }

    inline void read_all(std::string &data)
    {
        uint32_t size = file_size();
        if (size > 0) {
            data.resize(size);
            read(&data[0], size);
        }
    }

public:
    static bool is_file_exist(const char* file_path)
    {
#ifdef WIN32
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
    static bool is_directory_exist(const char* file_path)
    {
#ifdef WIN32
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
    static bool rename_director(const char* src_path, const char* des_path)
    {
        if (!is_directory_exist(src_path)) {
            return false;
        }
        return (rename(src_path, des_path) == 0);
    }
    static bool rename_file(const char* src_file_path, const char* des_file_path)
    {
        return (rename(src_file_path, des_file_path) == 0);
    }
    static bool remove_file(const char* file_path)
    {
        return (remove(file_path) == 0);
    }

public:
    static bool make_sure_directory_exist(const char* dir_path)
    {
        if (is_directory_exist(dir_path)) {
            return true;
        }
#ifdef WIN32
        return (MakeSureDirectoryPathExists(dir_path) == TRUE);
#else
        if (make_dirs(dir_path) == 0) {
            return true;
        }
#endif

        return false;
    }
#ifndef WIN32
    static int make_dirs(const char* dir_path)
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
                if (errno != ENOENT) {
                    break;
                }

                if ((rc = mkdir(saved_path, 0755)) < 0) {
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
    static std::string file_directory(const char* file_path)
    {
        std::string dir(file_path);
        int pos = -1;
#ifdef WIN32
        pos = dir.rfind('\\');
#else
        pos = dir.rfind('/');
#endif
        if (pos >= 0) {
            return dir.substr(0, pos + 1);
        }

        return "";
    }

private:
    FILE* m_fp;
    std::string path;
};

}
#endif