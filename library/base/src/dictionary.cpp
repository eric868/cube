/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: dictionary.cpp
Version: 1.0
Date: 2016.1.13

History:
base     2016.1.13   1.0     Create
******************************************************************************/

#include "dictionary.h"
#include <map>
#include <math.h>

namespace base {

dobject::dobject()
{
	value_type_ = DVT_NULL;
	value_.integer_value = 0;
}

dobject::dobject(bool value)
{
	value_type_ = DVT_BOOL;
	value_.bool_value = value;
}

dobject::dobject(int value)
{
	value_type_ = DVT_INTEGER;
	value_.integer_value = value;
}

dobject::dobject(dint value)
{
	value_type_ = DVT_INTEGER;
	value_.integer_value = value;
}

dobject::dobject(double value)
{
	value_type_ = DVT_DOUBLE;
	value_.double_value = value;
}

dobject::dobject(const char* value)
{
	value_type_ = DVT_STRING;
	if (value == NULL) {
		value_.string_value = new char[1];
		value_.string_value[0] = 0x00;
	}
	else {
		value_.string_value = new char[strlen(value) + 1];
		strcpy(value_.string_value, value);
	}
}

dobject::dobject(const std::string& value)
{
	value_type_ = DVT_STRING;
	value_.string_value = new char[strlen(value.c_str()) + 1];
	strcpy(value_.string_value, value.c_str());
}

dobject::dobject(const dictionary* value)
{
	assert(value != NULL);

	value_type_ = DVT_DICTIONARY;
	value_.dict_value = new dictionary(*value);
}

dobject::dobject(const dictionary& value)
{
	value_type_ = DVT_DICTIONARY;
	value_.dict_value = new dictionary(value);;
}

dobject::dobject(const darray* value)
{
	assert(value != NULL);

	value_type_ = DVT_ARRAY;
	value_.array_value = new darray(*value);
}

dobject::dobject(const darray& value)
{
	value_type_ = DVT_ARRAY;
	value_.array_value = new darray(value);
}

dobject::dobject(const dobject& rhs)
{
	value_type_ = rhs.value_type_;

	switch (rhs.value_type_) {
	case DVT_BOOL:
		value_.bool_value = rhs.value_.bool_value;
		break;
	case DVT_INTEGER:
		value_.integer_value = rhs.value_.integer_value;
		break;
	case DVT_DOUBLE:
		value_.double_value = rhs.value_.double_value;
		break;
	case DVT_STRING:
		value_.string_value = new char[strlen(rhs.value_.string_value) + 1];
		strcpy(value_.string_value, rhs.value_.string_value);
		break;
	case DVT_ARRAY:
		value_.array_value = new darray(*(rhs.value_.array_value));
		break;
	case DVT_DICTIONARY:
		value_.dict_value = new dictionary(*(rhs.value_.dict_value));
		break;
	default:
		break;
	}
}

dobject::~dobject()
{
	release_value();
}

dobject& dobject::operator=(bool value)
{
	if (value_type_ != DVT_BOOL) {
		release_value();
	}

	value_type_ = DVT_BOOL;
	value_.bool_value = value;
	return *this;
}

dobject& dobject::operator=(int value)
{
	if (value_type_ != DVT_INTEGER) {
		release_value();
	}

	value_type_ = DVT_INTEGER;
	value_.integer_value = value;
	return *this;
}

dobject& dobject::operator=(dint value)
{
	if (value_type_ != DVT_INTEGER) {
		release_value();
	}

	value_type_ = DVT_INTEGER;
	value_.integer_value = value;
	return *this;
}

dobject& dobject::operator=(double value)
{
	if (value_type_ != DVT_DOUBLE) {
		release_value();
	}

	value_type_ = DVT_DOUBLE;
	value_.double_value = value;
	return *this;
}

dobject& dobject::operator=(const char* value)
{
	release_value();
	
	value_type_ = DVT_STRING;
	if (value == NULL) {
		value_.string_value = new char[1];
		value_.string_value[0] = 0x00;
	}
	else {
		value_.string_value = new char[strlen(value) + 1];
		strcpy(value_.string_value, value);
	}
	return *this;
}

dobject& dobject::operator=(const std::string& value)
{
	release_value();

	value_type_ = DVT_STRING;
	value_.string_value = new char[strlen(value.c_str()) + 1];
	strcpy(value_.string_value, value.c_str());
	return *this;
}

dobject& dobject::operator=(const dictionary* value)
{
	assert(value != NULL);

	if (value_type_ != DVT_DICTIONARY) {
		release_value();
		value_.dict_value = new dictionary();
	}

	value_type_ = DVT_DICTIONARY;
	*(value_.dict_value) = *value;
	return *this;
}

dobject& dobject::operator=(const dictionary& value)
{
	if (value_type_ != DVT_DICTIONARY) {
		release_value();
		value_.dict_value = new dictionary();
	}

	value_type_ = DVT_DICTIONARY;
	*(value_.dict_value) = value;
	return *this;
}

dobject& dobject::operator=(const darray* value)
{
	assert(value != NULL);

	if (value_type_ != DVT_ARRAY) {
		release_value();
		value_.array_value = new darray();
	}

	value_type_ = DVT_ARRAY;
	*(value_.array_value) = *value;
	return *this;
}

dobject& dobject::operator=(const darray& value)
{
	if (value_type_ != DVT_ARRAY) {
		release_value();
		value_.array_value = new darray();
	}

	value_type_ = DVT_ARRAY;
	*(value_.array_value) = value;
	return *this;
}

dobject& dobject::operator=(const dobject& rhs)
{
	release_value();
	value_type_ = rhs.value_type_;

	switch (rhs.value_type_) {
	case DVT_BOOL:
		value_.bool_value = rhs.value_.bool_value;
		break;
	case DVT_INTEGER:
		value_.integer_value = rhs.value_.integer_value;
		break;
	case DVT_DOUBLE:
		value_.double_value = rhs.value_.double_value;
		break;
	case DVT_STRING:
		value_.string_value = new char[strlen(rhs.value_.string_value) + 1];
		strcpy(value_.string_value, rhs.value_.string_value);
		break;
	case DVT_ARRAY:
		value_.array_value = new darray(*rhs.value_.array_value);
		break;
	case DVT_DICTIONARY:
		value_.dict_value = new dictionary(*rhs.value_.dict_value);
		break;
	default:
		break;
	}
	return *this;
}

int dobject::value_type() const
{
	return value_type_;
}

bool dobject::is_null() const
{
	return value_type_ == DVT_NULL;
}

bool dobject::is_array() const
{
	return value_type_ == DVT_ARRAY;
}

bool dobject::is_dictionary() const
{
	return value_type_ == DVT_DICTIONARY;
}

bool dobject::bool_value() const
{
	assert(value_type_ != DVT_INVALID);

	switch (value_type_) {
	case DVT_BOOL:
		return value_.bool_value;
	case DVT_INTEGER:
		return value_.integer_value != 0;
	case DVT_DOUBLE:
		return value_.double_value != 0;
	case DVT_STRING:
		return atol(value_.string_value) != 0;
	default:
		return false;
	}
}

dint dobject::integer_value() const
{
	assert(value_type_ != DVT_INVALID);

	switch (value_type_) {
	case DVT_BOOL:
		return value_.bool_value;
	case DVT_INTEGER:
		return value_.integer_value;
	case DVT_DOUBLE:
		return (dint)value_.double_value;
	case DVT_STRING:
#ifdef OS32
		return atol(value_.string_value);
#else
		return atol(value_.string_value);
#endif
	default:
		return 0;
	}
}

int dobject::int_value() const
{
	assert(value_type_ != DVT_INVALID);

	switch (value_type_) {
	case DVT_BOOL:
		return value_.bool_value;
	case DVT_INTEGER:
		return (int)value_.integer_value;
	case DVT_DOUBLE:
		return (int)value_.double_value;
	case DVT_STRING:
#ifdef OS32
		return atoi(value_.string_value);
#else
		return atoi(value_.string_value);
#endif
	default:
		return 0;
	}
}

double dobject::double_value() const
{
	assert(value_type_ != DVT_INVALID);

	switch (value_type_) {
	case DVT_BOOL:
		return value_.bool_value;
	case DVT_INTEGER:
		return (double)value_.integer_value;
	case DVT_DOUBLE:
		return value_.double_value;
	case DVT_STRING:
		return atof(value_.string_value);
	default:
		return 0;
	}
}

std::string dobject::string_value(int float_precision) const
{
	if (value_type_ == DVT_NULL) {
		return "";
	}
	else if (value_type_ == DVT_STRING) {
		return value_.string_value;
	}
	else if (value_type_ == DVT_BOOL) {
		return value_.bool_value ? "true" : "false";
	}
	else if (value_type_ == DVT_INTEGER) {
		char sval[64];
#ifdef PLATFORM_WINDOWS
		_i64toa(value_.integer_value, sval, 10);
#else
		sprintf(sval, "%ld", value_.integer_value);
#endif
		return sval;
	}
	else if (value_type_ == DVT_DOUBLE) {
		char sval[512];
		if (float_precision < 0) {
			sprintf(sval, "%lf", value_.double_value);
		}
		else {
			char format[24];
			sprintf(format, "%%.%dlf", float_precision);
			sprintf(sval, format, value_.double_value);
		}
		return sval;
	}
	else if (value_type_ == DVT_ARRAY) {
		return value_.array_value->to_string();
	}
	else if (value_type_ == DVT_DICTIONARY) {
		return value_.dict_value->to_string();
	}
	else {
		return "";
	}
}

dictionary* dobject::dictionary_value()
{
	assert(value_type_ == DVT_DICTIONARY);

	if (value_type_ == DVT_DICTIONARY) {
		return value_.dict_value;
	}
	return NULL;
}

dictionary* dobject::dictionary_value() const
{
	assert(value_type_ == DVT_DICTIONARY);

	if (value_type_ == DVT_DICTIONARY) {
		return value_.dict_value;
	}
	return NULL;
}

darray* dobject::array_value()
{
	assert(value_type_ == DVT_ARRAY);

	if (value_type_ == DVT_ARRAY) {
		return value_.array_value;
	}
	return NULL;
}

darray* dobject::array_value() const
{
	assert(value_type_ == DVT_ARRAY);

	if (value_type_ == DVT_ARRAY) {
		return value_.array_value;
	}
	return NULL;
}

void dobject::release_value()
{
	if (value_type_ == DVT_STRING) {
		if (value_.string_value) {
			delete value_.string_value;
			value_.string_value = NULL;
		}
	}
	else if (value_type_ == DVT_DICTIONARY) {
		if (value_.dict_value) {
			delete value_.dict_value;
			value_.dict_value = NULL;
		}
	}
	else if (value_type_ == DVT_ARRAY) {
		if (value_.array_value) {
			delete value_.array_value;
			value_.array_value = NULL;
		}
	}
}

reference_base* dobject::retain()
{
	return reference_base::retain();
}

void dobject::release()
{
	reference_base::release();
}

std::string dobject::to_string() const
{
	std::string ret = string_value();
	if (value_type_ == DVT_STRING) {
		ret = "\"" + ret + "\"";
	}
	return ret;
}

void dobject::print() const
{
	printf("%s\n", to_string().c_str());
}

/*********************************************************************/

darray::darray()
{

}

darray::~darray()
{
	clear();
}

darray::darray(const darray& rhs)
{
	objs_.clear();
	for (int i = 0; i < (int)rhs.objs_.size(); i++) {
		dobject* obj = new dobject(*(rhs.objs_[i].get()));
		objs_.push_back(dobject_ref(obj));
		obj->release();
	}
}

darray& darray::operator=(const darray& rhs)
{
	objs_.clear();
	for (int i = 0; i < (int)rhs.objs_.size(); i++) {
		dobject* obj = new dobject(*(rhs.objs_[i].get()));
		objs_.push_back(dobject_ref(obj));
		obj->release();
	}
	return *this;
}

size_t darray::count() const
{
	return objs_.size();
}

bool darray::empty() const
{
	return (objs_.size() == 0);
}

dobject& darray::at(size_t index)
{
	assert(index < objs_.size());
	return *(objs_[index].get());
}

const dobject& darray::at(size_t index) const
{
	assert(index < objs_.size());
	return *(objs_[index].get());
}

dobject& darray::operator[](size_t index)
{
	assert(index < objs_.size());
	return *(objs_[index].get());
}

const dobject& darray::operator[](size_t index) const
{
	assert(index < objs_.size());
	return *(objs_[index].get());
}

void darray::append(bool value)
{
	dobject* obj = new dobject(value);
	objs_.push_back(dobject_ref(obj));
	obj->release();
}

void darray::append(int value)
{
	dobject* obj = new dobject(value);
	objs_.push_back(dobject_ref(obj));
	obj->release();
}

void darray::append(dint value)
{
	dobject* obj = new dobject(value);
	objs_.push_back(dobject_ref(obj));
	obj->release();
}

void darray::append(double value)
{
	dobject* obj = new dobject(value);
	objs_.push_back(dobject_ref(obj));
	obj->release();
}

void darray::append(const char* value)
{
	dobject* obj = new dobject(value);
	objs_.push_back(dobject_ref(obj));
	obj->release();
}

void darray::append(const std::string& value)
{
	dobject* obj = new dobject(value);
	objs_.push_back(dobject_ref(obj));
	obj->release();
}

void darray::append(const dictionary* value)
{
	assert(value != NULL);

	dobject* obj = new dobject(value);
	objs_.push_back(dobject_ref(obj));
	obj->release();
}

void darray::append(const dictionary& value)
{
	dobject* obj = new dobject(value);
	objs_.push_back(dobject_ref(obj));
	obj->release();
}

void darray::append(const darray* value)
{
	assert(value != NULL);

	dobject* obj = new dobject(value);
	objs_.push_back(dobject_ref(obj));
	obj->release();
}

void darray::append(const darray& value)
{
	dobject* obj = new dobject(value);
	objs_.push_back(dobject_ref(obj));
	obj->release();
}

void darray::append(const dobject& value)
{
	dobject* obj = new dobject(value);
	objs_.push_back(dobject_ref(obj));
	obj->release();
}

void darray::remove(size_t index)
{
	assert(index < objs_.size());
	objs_.erase(objs_.begin() + index);
}

void darray::clear()
{
	objs_.clear();
}

std::string darray::to_string() const
{
	std::string ret = "[";
	dvector::const_iterator it = objs_.begin();
	for (; it != objs_.end(); ++it) {
		if (it != objs_.begin()) {
			ret += ",";
		}
		ret += (*it)->to_string();
	}
	ret += "]";

	return ret;
}

void darray::print() const
{
	printf("%s\n", to_string().c_str());
}

/*********************************************************************/

dictionary::dictionary()
{

}

dictionary::~dictionary()
{
	clear();
}

dictionary::dictionary(const dictionary& rhs)
{
	map_objs_.clear();
	dmap::const_iterator mit = rhs.map_objs_.begin();
	for (; mit != rhs.map_objs_.end(); mit++) {
		dobject* obj = new dobject(*(mit->second.get()));
		map_objs_[mit->first] = dobject_ref(obj);
		obj->release();
	}
}

dictionary& dictionary::operator=(const dictionary& rhs)
{
	map_objs_.clear();
	dmap::const_iterator mit = rhs.map_objs_.begin();
	for (; mit != rhs.map_objs_.end(); mit++) {
		dobject* obj = new dobject(*(mit->second.get()));
		map_objs_[mit->first] = dobject_ref(obj);
		obj->release();
	}
	return *this;
}

size_t dictionary::count() const
{
	return map_objs_.size();
}

bool dictionary::empty() const
{
	return (map_objs_.size() == 0);
}

dobject& dictionary::operator[](size_t index)
{
	assert(index < map_objs_.size());
	
	dmap::iterator it = map_objs_.begin();
	while (index--) {
		++it;
	}
	return *it->second.get();
}

const dobject& dictionary::operator[](size_t index) const
{
	assert(index < map_objs_.size());

	dmap::const_iterator it = map_objs_.begin();
	while (index--) {
		++it;
	}
	return *it->second.get();
}

dobject& dictionary::operator[](const char* key)
{
	assert(key != NULL);

	dmap::iterator dit = map_objs_.find(key);
	if (dit == map_objs_.end()) {
		dobject* obj = new dobject();
		map_objs_[key] = dobject_ref(obj);
		obj->release();
	}
	return *map_objs_[key].get();
}

const dobject& dictionary::operator[](const char* key) const
{
	assert(key != NULL);

	dmap::const_iterator dit = map_objs_.find(key);
	if (dit != map_objs_.end()) {
		return *dit->second.get();
	}
	return dobject_null;
}

dobject& dictionary::operator[](const std::string& key)
{
	return (*this)[key.c_str()];
}

const dobject& dictionary::operator[](const std::string& key) const
{
	return (*this)[key.c_str()];
}

std::vector<std::string> dictionary::all_keys() const
{
	std::vector<std::string> keys;
	for (dmap::const_iterator dit = map_objs_.begin(); dit != map_objs_.end(); dit++) {
		keys.push_back(dit->first);
	}
	return keys;
}

void dictionary::add_object(const char* key, bool value)
{
	assert(key != NULL);

	dobject* obj = new dobject(value);
	map_objs_[key] = dobject_ref(obj);
	obj->release();
}

void dictionary::add_object(const char* key, int value)
{
	assert(key != NULL);

	dobject* obj = new dobject(value);
	map_objs_[key] = dobject_ref(obj);
	obj->release();
}

void dictionary::add_object(const char* key, dint value)
{
	assert(key != NULL);

	dobject* obj = new dobject(value);
	map_objs_[key] = dobject_ref(obj);
	obj->release();
}

void dictionary::add_object(const char* key, double value)
{
	assert(key != NULL);

	dobject* obj = new dobject(value);
	map_objs_[key] = dobject_ref(obj);
	obj->release();
}

void dictionary::add_object(const char* key, const char* value)
{
	assert(key != NULL);

	dobject* obj = new dobject(value);
	map_objs_[key] = dobject_ref(obj);
	obj->release();
}

void dictionary::add_object(const char* key, const dictionary* value)
{
	assert(key != NULL && value != NULL);

	dobject* obj = new dobject(value);
	map_objs_[key] = dobject_ref(obj);
	obj->release();
}

void dictionary::add_object(const char* key, const dictionary& value)
{
	assert(key != NULL);

	dobject* obj = new dobject(value);
	map_objs_[key] = dobject_ref(obj);
	obj->release();
}

void dictionary::add_object(const char* key, const darray* value)
{
	assert(key != NULL && value != NULL);

	dobject* obj = new dobject(value);
	map_objs_[key] = dobject_ref(obj);
	obj->release();
}

void dictionary::add_object(const char* key, const darray& value)
{
	assert(key != NULL);

	dobject* obj = new dobject(value);
	map_objs_[key] = dobject_ref(obj);
	obj->release();
}

void dictionary::add_object(const char* key, const dobject& value)
{
	assert(key != NULL);

	dobject* obj = new dobject(value);
	map_objs_[key] = dobject_ref(obj);
	obj->release();
}

void dictionary::add_object(const std::string& key, bool value)
{
	return add_object(key.c_str(), value);
}

void dictionary::add_object(const std::string& key, int value)
{
	return add_object(key.c_str(), value);
}

void dictionary::add_object(const std::string& key, dint value)
{
	return add_object(key.c_str(), value);
}

void dictionary::add_object(const std::string& key, double value)
{
	return add_object(key.c_str(), value);
}

void dictionary::add_object(const std::string& key, const char* value)
{
	return add_object(key.c_str(), value);
}

void dictionary::add_object(const std::string& key, const dictionary* value)
{
	return add_object(key.c_str(), value);
}

void dictionary::add_object(const std::string& key, const dictionary& value)
{
	return add_object(key.c_str(), value);
}

void dictionary::add_object(const std::string& key, const darray* value)
{
	return add_object(key.c_str(), value);
}

void dictionary::add_object(const std::string& key, const darray& value)
{
	return add_object(key.c_str(), value);
}

void dictionary::add_object(const std::string& key, const dobject& value)
{
	return add_object(key.c_str(), value);
}

void dictionary::remove_object(const char* key)
{
	dmap::iterator dit = map_objs_.find(key);
	if (dit != map_objs_.end()) {
		map_objs_.erase(dit);
	}
}

void dictionary::remove_object(const std::string& key)
{
	return remove_object(key.c_str());
}

void dictionary::clear()
{
	map_objs_.clear();
}

std::string dictionary::to_string() const
{
	std::string ret = "{";
	dmap::const_iterator dit = map_objs_.begin();
	for (; dit != map_objs_.end(); dit++) {
		if (dit != map_objs_.begin()) {
			ret += ",";
		}
		ret += "\"" + dit->first+ "\"";
		ret += ":";
		ret += dit->second->to_string();
	}
	ret += "}";

	return ret;
}

std::string dictionary::to_http_request_string() const
{
    std::string ret = "";
    dmap::const_iterator dit = map_objs_.begin();
    for (; dit != map_objs_.end(); dit++) {
        if (dit != map_objs_.begin()) {
            ret += "&";
        }
        ret += dit->first;
        ret += "=";
        ret += dit->second->string_value();
    }
    return ret;
}

void dictionary::print() const
{
	printf("%s\n", to_string().c_str());
}

}