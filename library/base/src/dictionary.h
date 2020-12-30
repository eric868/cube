/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: dictionary.h
Version: 1.0
Date: 2016.1.13

History:
base     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __VBASE_DICTIONARY_H__
#define __VBASE_DICTIONARY_H__

#include <vector>
#include <stdlib.h>
#include "base.h"
#include "reference_base.h"

namespace base {

typedef base::int64 dint;

enum DVALUE_TYPE
{
	DVT_INVALID = -1,
	DVT_NULL = 0,
	DVT_BOOL,
	DVT_INTEGER,
	DVT_DOUBLE,
	DVT_STRING,
	DVT_ARRAY,
	DVT_DICTIONARY,
};

class darray;
class dictionary;
union dvalue
{
	bool bool_value;
	dint integer_value;
	double double_value;
	char* string_value;
	darray* array_value;
	dictionary* dict_value;
};

/*********************************************************************/

class COMMON_API dobject
	: public reference_base
{
	friend class dobject_ref;
	friend class darray;
	friend class dictionary;

public:
	dobject();
	dobject(bool value);
	dobject(int value);
	dobject(dint value);
	dobject(double value);
	dobject(const char* value);
	dobject(const std::string& value);
	dobject(const dictionary* value);
	dobject(const dictionary& value);
	dobject(const darray* value);
	dobject(const darray& value);
	dobject(const dobject& rhs);
	~dobject();

public:
	dobject& operator=(bool value);
	dobject& operator=(int value);
	dobject& operator=(dint value);
	dobject& operator=(double value);
	dobject& operator=(const char* value);
	dobject& operator=(const std::string& value);
	dobject& operator=(const dictionary* value);
	dobject& operator=(const dictionary& value);
	dobject& operator=(const darray* value);
	dobject& operator=(const darray& value);
	dobject& operator=(const dobject& rhs);

public:
	int value_type() const;
	bool is_null() const;
	bool is_array() const;
	bool is_dictionary() const;

	bool bool_value() const;
	dint integer_value() const;
	int int_value() const;
	double double_value() const;
	std::string string_value(int float_precision = -1) const;
	dictionary* dictionary_value();
	dictionary* dictionary_value() const;
	darray* array_value();
	darray* array_value() const;

	std::string to_string() const;
	void print() const;

protected:
	void release_value();

	virtual reference_base* retain();
	virtual void release();

private:
	int value_type_;
	dvalue value_;
};

/*********************************************************************/

class dobject_ref
{
public:
	explicit dobject_ref(dobject* ptr = 0)
		: pointee_(ptr)
	{
		if (pointee_) {
			pointee_->retain();
		}
	}

	dobject_ref(const dobject_ref& rhs)
		: pointee_(rhs.get())
	{
		if (pointee_) {
			pointee_->retain();
		}
	}

	~dobject_ref() {
		if (pointee_) {
			pointee_->release();
		}
	}

	bool is_null() const {
		return pointee_ == NULL;
	}

	dobject_ref& operator= (const dobject_ref& rhs)
	{
		if (this != &rhs) {
			if (pointee_) {
				pointee_->release();
				pointee_ = NULL;
			}

			pointee_ = rhs.get();
			if (pointee_) {
				pointee_->retain();
			}
		}
		return *this;
	}

	bool operator== (const dobject_ref& rhs) {
		return pointee_ == rhs.pointee_;
	}

	bool operator!= (const dobject_ref& rhs) {
		return pointee_ != rhs.pointee_;
	}

	dobject& operator* () const {
		return *pointee_;
	}

	dobject* operator-> () const {
		return pointee_;
	}

	dobject* get() const {
		return pointee_;
	}

private:
	dobject* pointee_;
};

/*********************************************************************/

typedef std::vector<dobject_ref> dvector;
typedef VBASE_HASH_MAP<std::string, dobject_ref> dmap;
static const dobject dobject_null;

class COMMON_API darray
{
public:
	darray();
	~darray();

	darray(const darray& rhs);
	darray& operator=(const darray& rhs);

public:
	size_t count() const;
	bool empty() const;

	dobject& at(size_t index);
	const dobject& at(size_t index) const;
	dobject& operator[](size_t index);
	const dobject& operator[](size_t index) const;

public:
	void append(bool value);
	void append(int value);
	void append(dint value);
	void append(double value);
	void append(const char* value);
	void append(const std::string& value);
	void append(const dictionary* value);
	void append(const dictionary& value);
	void append(const darray* value);
	void append(const darray& value);
	void append(const dobject& value);
	void remove(size_t index);
	void clear();

	std::string to_string() const;
	void print() const;

private:
	dvector objs_;
};

/*********************************************************************/

class COMMON_API dictionary
{
public:
	dictionary();
	~dictionary();

	dictionary(const dictionary& rhs);
	dictionary& operator=(const dictionary& rhs);

public:
	size_t count() const;
	bool empty() const;

	dobject& operator[](size_t index);
	const dobject& operator[](size_t index) const;
	dobject& operator[](const char* key);
	const dobject& operator[](const char* key) const;
	dobject& operator[](const std::string& key);
	const dobject& operator[](const std::string& key) const;

	std::vector<std::string> all_keys() const;

public:
	void add_object(const char* key, bool value);
	void add_object(const char* key, int value);
	void add_object(const char* key, dint value);
	void add_object(const char* key, double value);
	void add_object(const char* key, const char* value);
	void add_object(const char* key, const dictionary* value);
	void add_object(const char* key, const dictionary& value);
	void add_object(const char* key, const darray* value);
	void add_object(const char* key, const darray& value);
	void add_object(const char* key, const dobject& value);

	void add_object(const std::string& key, bool value);
	void add_object(const std::string& key, int value);
	void add_object(const std::string& key, dint value);
	void add_object(const std::string& key, double value);
	void add_object(const std::string& key, const char* value);
	void add_object(const std::string& key, const dictionary* value);
	void add_object(const std::string& key, const dictionary& value);
	void add_object(const std::string& key, const darray* value);
	void add_object(const std::string& key, const darray& value);
	void add_object(const std::string& key, const dobject& value);

	void remove_object(const char* key);
	void remove_object(const std::string& key);
	void clear();

	std::string to_string() const;
	std::string to_http_request_string() const;
	void print() const;

private:
	dmap map_objs_;
};

typedef ref_adapter<base::dictionary> ref_dictionary;

}
#endif
