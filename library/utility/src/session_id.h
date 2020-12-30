/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: session_id.h
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __UTILITY_SESSION_ID_H__
#define __UTILITY_SESSION_ID_H__

#include "utility.h"

namespace utility 
{

class COMMON_API session_id
{
public:
	session_id() {}
	session_id(const session_id& other);
	explicit session_id(int id) 
		: id_(id)
	{
	}
	session_id operator=(const session_id& other);

	int get_id() const { return id_; }
	void set_id(int value) { id_ = value; }

	// ppeator < for map search
	bool operator < (const session_id& other) const {
		if (id_ < other.get_id()) {
			return true;
		}
		return false;
	}

private:
	int id_;
};

COMMON_API bool operator==(const session_id& rhs, const session_id& lhs);

}
#endif
