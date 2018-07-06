#ifndef DAUNTLESS_ASS_OPTIONS_HPP
#define DAUNTLESS_ASS_OPTIONS_HPP

#include "gear/Armoury.hpp"
#include "gear/Cell.hpp"
#include <string>
#include <unordered_map>
#include <array>

#define CONFIG_FILE_NAME "setting.json"
#define LAST_SEARCH "lastSearch.ass"

class Options
{
  public:
    struct SkillSearch
    {
        Gear::SkillType filter;
        std::string skillName;
        int skillLevel;
    };
    void load(const Gear::Armoury &armoury, const std::string &fileName = CONFIG_FILE_NAME);
    void save(const std::string &fileName = CONFIG_FILE_NAME);
    void loadSearch(const std::string &fileName = LAST_SEARCH);
    void saveSearch(const std::string &fileName = LAST_SEARCH);

    int numberOfResults = 1000;
    std::string language = "English";
    std::unordered_map<std::string, bool> checkedGear;
    std::unordered_map<Gear::Cell, int> cells;
    std::array<SkillSearch, 7> skillSearches;
    std::vector<Gear::ArmourSet> armourSets;
    Gear::WeaponType weaponType;
    
};

#endif // !DAUNTLESS_ASS_OPTIONS_HPP