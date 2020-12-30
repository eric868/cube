/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: md5.cpp
Version: 1.0
Date: 2016.1.13

History:
base     2016.1.13   1.0     Create
******************************************************************************/

#include "md5.h"
#include <openssl/md5.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#ifndef _MSC_VER
#include <dirent.h>
#endif
#include "trace.h"
#include <algorithm>
#include <vector>

namespace base
{

std::string md5(const char* text, int length)
{
	assert(text != NULL);
	assert(*text != 0x00);

	if (length == -1) {
		length = strlen(text);
	}

	unsigned char c[MD5_DIGEST_LENGTH];

	MD5_CTX context;
	MD5_Init(&context);
	MD5_Update(&context, text, length);
	MD5_Final(c, &context);

	char result[MD5_DIGEST_LENGTH * 4];
	for(int i = 0; i < MD5_DIGEST_LENGTH; i++) {
		sprintf(&(result[i * 2]), "%02x", (unsigned int)c[i]);
	}
	return result;
}

int md5(std::string& output, const char* text, int length)
{
	output = md5(text, length);
	return output.length() > 0 ? 0 : -1;
}

std::string md5_file(const char* file_path)
{
	FILE* file = NULL;
	file = fopen(file_path, "rb");
	if(file == NULL) {
	    TRACE_ERROR("base", 0, "md5 - file is not found '%s'", file_path);
		return "";
	}

	int bytes;
	MD5_CTX context;
	unsigned char data[1024];
	unsigned char c[MD5_DIGEST_LENGTH];

	MD5_Init(&context);
	while((bytes = fread(data, 1, 1024, file)) != 0) {
		MD5_Update(&context, data, bytes);
	}

	fclose(file);
	MD5_Final(c, &context);

	char result[MD5_DIGEST_LENGTH * 4];
	for(int i = 0; i < MD5_DIGEST_LENGTH; i++) {
		sprintf(&(result[i * 2]), "%02x", (unsigned int)c[i]);
	}
	return result;
}

int md5_file(std::string& output, const char* file_path)
{
	output = md5_file(file_path);
	return output.length() > 0 ? 0 : -1;
}

#if 0
#if defined(_MSC_VER)
void traveFile(stemp,file_path,type)
{

}
#else
void ListFile(std::string &output,const char* path,const char* type)
{
	 size_t nsize = strlen(type);
    struct dirent* ent = NULL;
    DIR *pDir;
    pDir=opendir(path);
    while (NULL != (ent=readdir(pDir)))
    {
    	std::string sname(ent->d_name);
    	if (sname.compare(".") == 0 || sname.compare("..") == 0 || sname.empty())
    	{
    		continue;
    	}
    	if(ent->d_type == 8)
    	{
    		if(sname.size()<nsize)
    		{
    			continue;
    		}
    		else
    		{
    			//printf("sname=%s\n",sname.substr(sname.size()-nsize,nsize).c_str());
    			if(sname.compare(sname.size()-nsize,nsize,type) == 0)
    			{
    				std::string stemp;
    				std::string sfile(path);
    				sfile += '/';
    				sfile += ent->d_name;
    				md5_file(stemp, sfile.c_str());
    				output += stemp;
    			}
    		}
    	}
    	else if(ent->d_type == 4)
    	{
    		std::string stemp;
    		std::string sfile(path);
    		sfile += '/';
    		sfile += ent->d_name;
    		//printf("sfile=%s\n",sfile.c_str());
    		ListFile(stemp,sfile.c_str(),type);
    		output += stemp;
    	}
    }
}
#endif
int calcul_path_md5(std::string& output, const char* file_path,const char* type)
{
	std::string stemp;
	assert(file_path != NULL);
	assert(type != NULL);
#if defined(_MSC_VER)
	traveFile(stemp,file_path,type);
#else
	ListFile(stemp,file_path,type);
#endif
	assert(stemp.c_str() != NULL);
	printf("value=%s\n",stemp.c_str());
	md5(output,stemp.c_str(),-1);
	return 0;
}
#endif

#if defined(_MSC_VER)
void traveFile(std::string &output, const char* path)
{
	//do nothing
}
#else
void ListFile(std::string &output, const char* path, const std::string stype)
{
    struct dirent* ent = NULL;
    DIR *pDir;
    pDir=opendir(path);
    if(pDir == NULL)
    {
    	printf("unusable path:%s\n",path);
    	return;
    }
    while (NULL != (ent=readdir(pDir)))
    {
    	std::vector<std::string> vsname;
    	std::string sname(ent->d_name);
    	if (sname.compare(".") == 0 || sname.compare("..") == 0 || sname.empty())
    	{
    		continue;
    	}
    	if(ent->d_type == 8)
    	{
    		std::string sname(ent->d_name);
    		int npos = sname.find_last_of('.');
    		if(npos == -1)
    		{
    			continue;
    		}
    		std::string sfiletype = sname.substr(npos,sname.size() - npos);
    		sfiletype = '$' + sfiletype + '$';
    		if(stype.find(sfiletype.c_str()) == std::string::npos)
    		{
    			continue;
    		}
    		vsname.push_back(ent->d_name);
    	}
    	std::sort(vsname.begin(), vsname.end());
    	std::string stemp;
    	for(size_t i = 0; i < vsname.size(); i++)
    	{
    		std::string sfile(path);
    		sfile += '/';
    		sfile += vsname.at(i).c_str();
    		stemp.clear();
    		md5_file(stemp, sfile.c_str());
    		output += stemp;
    	}

    }
}
#endif

int calcul_path_md5(std::string& output, const char* file_path, const std::string stype)
{
	std::string stemp;
	assert(file_path != NULL);
#if defined(_MSC_VER)
	//traveFile(stemp, file_path, stype);
    traveFile(stemp, file_path);
#else
	ListFile(stemp, file_path, stype);
#endif
	if(stemp.empty())
	{
		printf("the file_path maybe not a valued path or there is not suitable file that you need."
				" file_path:%s\n",file_path);
		return -1;
	}
	md5(output, stemp.c_str(),-1);
	return 0;
}
}
