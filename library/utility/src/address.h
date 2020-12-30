/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: address.h
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __UTILITY_ADDRESS_H__
#define __UTILITY_ADDRESS_H__

#include "utility.h"
#include <string>

namespace utility {

struct COMMON_API address {  
    std::string host_name;
    std::string port;
};
}
#endif
