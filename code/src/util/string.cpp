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
