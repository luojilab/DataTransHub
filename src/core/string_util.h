#ifndef STRUTIL_H_
#define STRUTIL_H_

#include <string>
#include <vector>

namespace future {
    std::string &TrimLeft(std::string &str);

    std::string &TrimRight(std::string &str);

    std::string &Trim(std::string &str);

    std::string &ToLower(std::string &str);

    std::string &ToUpper(std::string &str);

    bool StartsWith(const std::string &str, const std::string &substr);

    bool EndsWith(const std::string &str, const std::string &substr);

    std::int64_t StrToInt64(const std::string &in);

    std::string Int64ToStr(std::int64_t in);

    void StringReplace(std::string &srcStr, const std::string &findStr,const std::string &replaceStr );

}
#endif// STRUTIL_H_
