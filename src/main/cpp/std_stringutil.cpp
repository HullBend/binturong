

#include "std_stringutil.h"


static
void split_(std::vector<std::string>& stringVecOut,
            const std::string& str,
            const std::string& delimiter,
            bool includeEmptyStrings)
{
    if (delimiter.empty() || str.empty()) {
        stringVecOut.push_back(str);
        return;
    }

    size_t start = 0, end = 0, length = 0;

    while (end != std::string::npos) {
        end = str.find(delimiter, start);
        // If at end, use length = maxLength. Else use length = end - start.
        length = (end == std::string::npos) ? std::string::npos : (end - start);
        if (includeEmptyStrings || (length > 0 && (start  < str.size()))) {
            /* At end, end == length == string::npos */
            stringVecOut.push_back(str.substr(start, length));
        }
        // If at end, use start = maxSize. Else use start = end + delimiter.
        start = (end > (std::string::npos - delimiter.size())) ? std::string::npos : (end + delimiter.size());
    }
}


const std::vector<std::string> stringSplit(const std::string& str,
        const std::string& delimiter,
        bool includeEmptyStrings)
{
    std::vector<std::string> vec;
    split_(vec, str, delimiter, includeEmptyStrings);
    return vec;
}


const std::string
stringReplaceAll(std::string& source, const std::string& find, const std::string& replace)
{
    if (source.empty() || find.empty()) {
        return source;
    }

    size_t pos = 0;
    while ((pos = source.find(find, pos)) != std::string::npos) {
        source.replace(pos, find.length(), replace);
        pos += replace.length();
    }

    return source;
}


bool stringEndsWith(const std::string& str, const char toFind) {
    if (str.empty()) {
        return false;
    }
    return (str[str.length() - 1] == toFind);
}
