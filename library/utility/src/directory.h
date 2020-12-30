/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: directory.h
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __UTILITY_DIRECTORY_H__
#define __UTILITY_DIRECTORY_H__ 

#include <string>
#include <set>

#ifdef WIN32
#include <direct.h>
#include <io.h>
#include <windows.h>
#include <tchar.h>
#else
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#endif
#include "utility.h"
using namespace std;

namespace utility {

#ifdef WIN32
#define ACCESS _access
#define MKDIR(a) _mkdir((a))
#else
#define ACCESS access
#define MKDIR(a) mkdir((a),0755)
#endif

	bool COMMON_API is_dir_exist(const string& dir_path);

	bool COMMON_API make_dir(const string& dirpath);

	bool COMMON_API get_files_indir(string path, set<string>& files);
}
#endif
