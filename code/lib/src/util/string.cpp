#include "util/string.hpp"
#include <algorithm>

std::vector<std::string> util::string::split(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

bool util::string::isComment(const std::string &line) { return line.empty() || line[0] == '#'; }

void util::string::toLower(std::string &str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

std::string util::string::toLowerCopy(std::string str)
{
    toLower(str);
    return str;
}

// taken from SO: https://stackoverflow.com/a/4643526
void util::string::replace(std::string &str, const std::string &subString,
                           const std::string &replaceString)
{
    size_t index = 0;
    while (true)
    {
        /* Locate the substring to replace. */
        index = str.find(subString, index);
        if (index == std::string::npos)
            break;

        /* Make the replacement. */
        str.replace(index, subString.length(), replaceString);

        /* Advance index forward so the next iteration doesn't pick it up as well. */
        index += replaceString.length();
    }
}
std::string util::string::vectorJoin(const std::vector<std::string> &vector,
                                     const std::string &separator)
{
    std::stringstream ss;
    for (size_t i = 0; i < vector.size(); ++i)
    {
        ss << vector[i];
        if (i != vector.size() - 1)
            ss << separator;
    }
    return ss.str();
}
