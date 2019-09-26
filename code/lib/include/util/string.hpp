#ifndef DAUNTLESS_ASS_UTIL_STRING_HPP
#define DAUNTLESS_ASS_UTIL_STRING_HPP

#include <iterator>
#include <sstream>
#include <string>
#include <vector>

namespace util
{
namespace string
{

// taken from here: https://stackoverflow.com/a/236803
template <typename Out> void split(const std::string &s, char delim, Out result)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
    {
        *(result++) = item;
    }
}

std::vector<std::string> split(const std::string &s, char delim);

bool isComment(const std::string &line);

void toLower(std::string &str);
std::string toLowerCopy(std::string str);

void replace(std::string &str, const std::string &subString, const std::string &replaceString);

std::string vectorJoin(const std::vector<std::string> &vector, const std::string &separator);

} // namespace string
} // namespace util

#endif // !DAUNTLESS_ASS_UTIL_STRING_HPP