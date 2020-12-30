/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: directory.cpp
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#include "directory.h"

namespace utility {
	bool is_dir_exist(const string& dir_path)
	{
		return ACCESS(dir_path.c_str(), 0) != -1;
	}

	bool make_dir(const string& dirpath)
	{
		string pathtemp;
		pathtemp = dirpath;
		string tempdir;
		while (pathtemp.find('\\') != string::npos) {
			pathtemp.replace(pathtemp.find('\\'), 1, "/");
		}

		if (pathtemp[pathtemp.length() - 1] != '/') {
			pathtemp += "/";
		}

		while (pathtemp.find('/') != string::npos) {
			tempdir.insert(tempdir.length(), pathtemp, 0, pathtemp.find('/') + 1);
			pathtemp = pathtemp.substr(pathtemp.find('/') + 1);
			int result = 0;
			if (!is_dir_exist(tempdir) && (0 != MKDIR(tempdir.c_str()))) {
				return false;
			}
		}

		return true;
	}

	bool get_files_indir(string path, set<string>& files) {
		while (path.find('\\') != string::npos) {
			path.replace(path.find('\\'), 1, "/");
		}

		if (!is_dir_exist(path)) { return false; }

#ifdef WIN32
		_finddata_t info;
		if (path[path.length() - 1] != '/') {
			path += "/*";
		} else {
			path += "*";
		}
		long handle = _findfirst(path.c_str(), &info);

		if (handle == -1L) { return false; }

		do {
			if (!(info.attrib & _A_SUBDIR)) { files.insert(info.name); }
		} while (_findnext(handle, &info) == 0);

		_findclose(handle);
#else
		DIR              *pDir ;  
		struct dirent    *ent  ;  
		int               i=0  ;  

		pDir=opendir(path.c_str());  

		while((ent=readdir(pDir))!=NULL) {  
			if(!(ent->d_type & DT_DIR)) { 
				files.insert(ent->d_name); 
			}
		} 
#endif

		return true;
	}
}