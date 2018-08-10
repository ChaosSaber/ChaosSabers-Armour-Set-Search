#ifndef DAUNTLESS_ASS_OPTIONS_HPP
#define DAUNTLESS_ASS_OPTIONS_HPP

#include "Dictionary.hpp"
#include "gear/ArmourSet.hpp"
#include "gear/Armoury.hpp"
#include "gear/Cell.hpp"
#include "util/files.hpp"
#include <QDir>
#include <QJsonObject>
#include <array>
#include <string>
#include <unordered_map>

#define NUMBER_OF_SKILLSELECTORS 7

class OptionsIoException
{
  public:
    OptionsIoException(std::string what) : what(std::move(what)) {}
    std::string what;
};

const QString CONFIG_FILE_NAME =
    util::files::pathAppend(util::files::DOCUMENTS_LOCATION, "setting.json");
const QString LAST_SEARCH =
    util::files::pathAppend(util::files::DOCUMENTS_LOCATION, "lastSearch.ass");
const QString STANDARD_CELL_SAVE_LOCATION =
    util::files::pathAppend(util::files::DOCUMENTS_LOCATION, "cells");
const QString MY_CELLS = util::files::pathAppend(STANDARD_CELL_SAVE_LOCATION, "myCells.cells");
const QString STANDARD_SEARCH_SAVE_LOCATION =
    util::files::pathAppend(util::files::DOCUMENTS_LOCATION, "searches");

class Options
{

  public:
    Options();
    struct SkillSearch
    {
        Gear::SkillType filter = Gear::SkillType::None;
        std::string skillName = "";
        int skillLevel = 0;
    };
    void loadConfiguration(const Gear::Armoury &armoury,
                           const QString &fileName = CONFIG_FILE_NAME);
    void saveConfiguration(const QString &fileName = CONFIG_FILE_NAME);
    void loadSearch(const Gear::Armoury &armoury, const Dictionary &dict,
                    const QString &fileName = LAST_SEARCH);
    void saveSearch(const QString &fileName = LAST_SEARCH);
    void save();
    void saveCells(const QString &fileName = MY_CELLS);
    void loadCells(const Gear::Armoury &armoury, const Dictionary &dict,
                   const QString &fileName = MY_CELLS);

    int numberOfResults = 100;
    std::string language = "English";
    std::unordered_map<std::string, bool> checkedGear;
    std::unordered_map<Gear::Cell, int> cells;
    std::array<SkillSearch, NUMBER_OF_SKILLSELECTORS> skillSearches;
    std::vector<Gear::ArmourSet> armourSets;
    Gear::WeaponType weaponType = Gear::WeaponType::Sword;
    QString lastSearchSaveLocation = STANDARD_SEARCH_SAVE_LOCATION;
    QString lastCellSaveLocation = STANDARD_CELL_SAVE_LOCATION;
    int cellUsage = 0;
    bool useLowerTierArmour = true;
    int tier = 6;

  private:
    QJsonObject cellToJson(const Gear::Cell &cell);
    Gear::Cell jsonToCell(const QJsonObject &json, const Gear::Armoury &armoury,
                          const Dictionary &dict, bool useDict);
    QJsonObject armourToJson(const Gear::Armour &armour);
    Gear::Armour jsonToArmour(const QJsonObject &json, const Gear::Armoury &armoury,
                              const Dictionary &dict, bool useDict);
    QJsonObject weaponToJson(const Gear::Weapon &weapon);
    Gear::Weapon jsonToWeapon(const QJsonObject &json, const Gear::Armoury &armoury,
                              const Dictionary &dict, bool useDict);
};

#endif // !DAUNTLESS_ASS_OPTIONS_HPP