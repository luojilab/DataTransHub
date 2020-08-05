
#include "string_util.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <algorithm>
#include <sstream>

#ifdef WIN32
#define snprintf _snprintf
#endif

namespace future {

#define TRIMLEFT(T) T& TrimLeft(T& str)\
{\
    T& t = str;\
    T::iterator i;\
    for (i = t.begin(); i != t.end(); ++i)\
    {\
        if (!isspace((int)*i))\
        {\
            break;\
        }\
    }\
    if (i == t.end())\
    {\
        t.clear();\
    }\
    else\
    {\
        t.erase(t.begin(), i);\
    }\
    return t;\
}

#define TRIMRIGHT(T) T& TrimRight(T& str)\
{\
    if (str.begin() == str.end())\
    {\
        return str;\
    }\
\
    T& t = str;\
    T::iterator i;\
    for (i = t.end() - 1;; --i)\
    {\
        if (!isspace((int)*i))\
        {\
            t.erase(i + 1, t.end());\
            break;\
        }\
        if (i == t.begin())\
        {\
            t.clear();\
            break;\
        }\
    }\
    return t;\
}

#define TRIM(T) T& Trim(T& str)\
{\
    T& t = str;\
\
    T::iterator i;\
    for (i = t.begin(); i != t.end(); ++i)\
    {\
        if (!isspace((int)*i))\
        {\
            break;\
        }\
    }\
    if (i == t.end())\
    {\
        t.clear();\
        return t;\
    }\
    else\
    {\
        t.erase(t.begin(), i);\
    }\
\
    for (i = t.end() - 1;; --i)\
    {\
        if (!isspace((int)*i))\
        {\
            t.erase(i + 1, t.end());\
            break;\
        }\
        if (i == t.begin())\
        {\
            t.clear();\
            break;\
        }\
    }\
\
    return t;\
}

#define TOLOWER(T) T& ToLower(T& str)\
{\
    T& t = str;\
    std::transform(t.begin(), t.end(), t.begin(), ::tolower);\
    return t;\
}

#define TOUPPER(T) T& ToUpper(T& str)\
{\
    T& t = str;\
    std::transform(t.begin(), t.end(), t.begin(), ::toupper);\
    return t;\
}

#define STARTSWITH(T) bool StartsWith(const T& str, const T& substr)\
{\
    return str.find(substr) == 0;\
}

#define ENDSWITH(T) bool EndsWith(const T& str, const T& substr)\
{\
    size_t i = str.rfind(substr);\
    return (i != T::npos) && (i == (str.length() - substr.length()));\
}

    TRIMLEFT(std::string)

    TRIMRIGHT(std::string)

    TRIM(std::string)

    TOLOWER(std::string)

    TOUPPER(std::string)

    STARTSWITH(std::string)

    ENDSWITH(std::string)

    std::string Int64ToStr(std::int64_t in) {
        std::stringstream stream;
        stream << in;
        return std::move(stream.str());
    }

    std::int64_t StrToInt64(const std::string &in) {
        std::int64_t out = 0;
        std::stringstream stream(in);
        stream >> out;
        return out;
    }

    void StringReplace(std::string &srcStr, const std::string &findStr,const std::string &replaceStr ) {
        std::string::size_type pos = 0;
        std::string::size_type srcLen = srcStr.size();
        std::string::size_type findStrLen = findStr.size();
        pos = srcStr.find(findStr, pos);
        while ((pos != std::string::npos)) {
            srcStr.replace(pos, findStrLen, replaceStr);
            pos = srcStr.find(findStr, (pos + findStrLen));
        }
    }

}
