/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: session_id.cpp
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#include "session_id.h"
namespace utility
{
	session_id::session_id(const session_id& other) {
		id_ = other.id_;
	}

	session_id session_id::operator=(const session_id& other) {
		if (this == &other) {
			return *this;
		}
		id_ = other.id_;
		return *this;
	}

	bool operator==(const session_id& rhs, const session_id& lhs) {
		return (rhs.get_id() == lhs.get_id());
	}
}
