#ifndef DAUNTLESS_ASS_OPTIONS_HPP
#define DAUNTLESS_ASS_OPTIONS_HPP

#include "gear/Armoury.hpp"
#include "gear/Cell.hpp"
#include <string>
#include <unordered_map>

#define CONFIG_FILE_NAME "setting.json"

class Options
{
  public:
    void load(const Gear::Armoury &armoury, const std::string &fileName = CONFIG_FILE_NAME);
    void save(const std::string &fileName = CONFIG_FILE_NAME);

    int numberOfResults = 1000;
    std::string language = "English";
    std::unordered_map<std::string, bool> checkedGear;
    std::unordered_map<Gear::Cell, int> cells;
};

#endif // !DAUNTLESS_ASS_OPTIONS_HPP