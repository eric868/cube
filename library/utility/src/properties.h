/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: properties.h
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/


#ifndef __UTILITY_PROPERTIES_H__
#define __UTILITY_PROPERTIES_H__

#include <map>
#include <vector>
#include "utility.h"

using namespace std;

namespace utility {

	typedef map<string, string> properties_dict;

	// Read Properites config file.
	class COMMON_API properties 
	{
	public:
		// Default constructor.
		properties();
		properties(const string& file);
		// Copy constructor
		properties(const properties& other);
		~properties();

		// @exception: if group or key isn't exist, then return string()
		string get_properties_string(const string& group, const string& key);

		// @exception: if group or key isn't exist, then return 0
		int get_groperties_int(const string& group, const string& key);

		// @exception: if group or key isn't exist, then return 0
		float get_groperties_float(const string& group, const string& key);

		// @exception: if group or key isn't exist, then return 0
		double get_groperties_double(const string& group, const string& key);

		// @return: if group and key is exist, return properties value of
		//			vector<string> size. else return 0.
		int get_groperties_stringlist(const string& group, const string& key,
			vector<string>* result);

		// @return: if group and key is exist, return properties value of
		//			vector<int> size. else return 0.
		int get_groperties_intlist(const string& group, const string& key,
			vector<int>* result);

		// @return: if group and key is exist, return properties value of
		//			vector<float> size. else return 0.
		int get_groperties_floatlist(const string& group, const string& key,
			vector<float>* result);

		// @return: if group and key is exist, return properties value of
		//			vector<double> size. else return 0.
		int get_groperties_doubleList(const string& group, const string& key,
			vector<double>* result);

		properties_dict get_groperties_dict_by_group(const string& group);

		// Load properties file.
		// @param file: file name (include file path)
		void load(const string& file);

	private:
		// Parse config file a line
		// @param line: properties file a line.
		void parse_line(const string& line);

		// Add a new properties group to parameters_. if group is exist that
		// wouldn't add.
		// @param group: new add group name.
		void add_group(const string& group);

		// Add a new properties to specific PropertiesDict. if key is exist. then
		// wouldn't add.
		// @param key: new add key name.
		// @param value: key's value.
		void add_properties(const string& key, const string& value);

		properties_dict *current_properties_dict_;

		// Parameters of map.
		map<string, properties_dict> parameters_;
	};
}
#endif
