#ifndef DAUNTLESS_ASS_OPTIONS_HPP
#define DAUNTLESS_ASS_OPTIONS_HPP

#include "gear/ArmourSet.hpp"
#include "gear/Armoury.hpp"
#include "gear/Cell.hpp"
#include <QDir>
#include <QJsonObject>
#include <array>
#include <string>
#include <unordered_map>

#define CONFIG_FILE_NAME "setting.json"
#define LAST_SEARCH "lastSearch.ass"
#define MY_CELLS_OLD "data/myCells.json"
#define MY_CELLS "data/myCells.cells"
#define NUMBER_OF_SKILLSELECTORS 7

class OptionsIoException
{
  public:
    OptionsIoException(std::string what) : what(std::move(what)) {}
    std::string what;
};

class Options
{
  public:
    struct SkillSearch
    {
        Gear::SkillType filter;
        std::string skillName;
        int skillLevel;
    };
    void loadConfiguration(const Gear::Armoury &armoury,
                           const std::string &fileName = CONFIG_FILE_NAME);
    void saveConfiguration(const std::string &fileName = CONFIG_FILE_NAME);
    void loadSearch(const Gear::Armoury &armoury, const std::string &fileName = LAST_SEARCH);
    void saveSearch(const std::string &fileName = LAST_SEARCH);
    void save();
    void load(const Gear::Armoury &armoury);
    void saveCells(const std::string &fileName = MY_CELLS);
    void loadCells(const Gear::Armoury &armoury, std::string fileName = MY_CELLS);

    int numberOfResults = 100;
    std::string language = "English";
    std::unordered_map<std::string, bool> checkedGear;
    std::unordered_map<Gear::Cell, int> cells;
    std::array<SkillSearch, NUMBER_OF_SKILLSELECTORS> skillSearches;
    std::vector<Gear::ArmourSet> armourSets;
    Gear::WeaponType weaponType;
    QString lastSaveLocation = QDir::currentPath();
    int cellUsage = 0;

  private:
    int version;

    QJsonObject cellToJson(const Gear::Cell &cell);
    Gear::Cell jsonToCell(const QJsonObject &json, const Gear::Armoury &armoury);
    QJsonObject armourToJson(const Gear::Armour &armour);
    Gear::Armour jsonToArmour(const QJsonObject &json, const Gear::Armoury &armoury);
    QJsonObject weaponToJson(const Gear::Weapon &weapon);
    Gear::Weapon jsonToWeapon(const QJsonObject &json, const Gear::Armoury &armoury);
};

#endif // !DAUNTLESS_ASS_OPTIONS_HPP