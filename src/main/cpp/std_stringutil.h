

#ifndef __STD_STRING_UTIL_H__
#define __STD_STRING_UTIL_H__


#ifndef STDAFX_INCLUDED_
#include "stdafx.h"
#endif /* STDAFX_INCLUDED_ */




__GCC_DONT_EXPORT
const std::vector<std::string>
stringSplit(const std::string& str, const std::string& delimiter, bool includeEmptyStrings = false);


__GCC_DONT_EXPORT
const std::string
stringReplaceAll(std::string& source, const std::string& toFind, const std::string& replace);


__GCC_DONT_EXPORT
bool stringEndsWith(const std::string& str, const char toFind);


#endif /* __STD_STRING_UTIL_H__ */
