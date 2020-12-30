/******************************************************************************
Copyright (c) 2020. All Rights Reserved.

FileName: memoryex.h
Version: 1.0
Date: 2016.1.13

History:
eric     2016.4.13   1.0     Create
******************************************************************************/

#ifndef MEMORYEX_H_
#define MEMORYEX_H_

#include <string>

namespace eco {
template< class StringVector, class StringType, class DelimType>
inline void StringSplit(
    IN const StringType& str,
    IN const DelimType& delims,
    IN unsigned int maxSplits,
    OUT StringVector& ret)
{
    if (str.empty()) {
        return;
    }

    unsigned int numSplits = 0;

    // Use STL methods
    size_t start, pos;
    start = 0;

    do {
        pos = str.find_first_of(delims, start);

        if (pos == start) {
            ret.push_back(StringType());
            start = pos + 1;
        }
        else if (pos == StringType::npos || (maxSplits && numSplits + 1 == maxSplits)) {
            // Copy the rest of the string
            ret.emplace_back(StringType());
            *(ret.rbegin()) = StringType(str.data() + start, str.size() - start);
            break;
        }
        else {
            // Copy up to delimiter
            //ret.push_back( str.substr( start, pos - start ) );
            ret.push_back(StringType());
            *(ret.rbegin()) = StringType(str.data() + start, pos - start);
            start = pos + 1;
        }

        ++numSplits;

    } while (pos != StringType::npos);
}

inline void StringReplace(std::string& str, const std::string& src, const std::string& dest)
{
    std::string::size_type pos = 0;
    std::string::size_type srclen = src.size();
    std::string::size_type deslen = dest.size();
    pos = str.find(src, pos);
    while ((pos != std::string::npos)) {
        str.replace(pos, srclen, dest);
        pos = str.find(src, (pos + deslen));
    }
}

inline std::string& StringTrim(std::string &str)
{
    if (str.empty()) {
        return str;
    }
    str.erase(0, str.find_first_not_of(" "));
    str.erase(str.find_last_not_of(" ") + 1);
    return str;
}

}
#endif
