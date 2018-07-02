#include "util/string.hpp"

std::vector<std::string> util::string::split(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

bool util::string::isComment(const std::string &line) { return line.empty() || line[0] == '#'; }