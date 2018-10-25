#include "Options.hpp"
#include "util/json.hpp"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <iostream>
#include <sstream>

#define CURRENT_VERSION 4

const QString VERSION = "Version";
const QString MAX_RESULTS = "Results";
const QString LANGUAGE = "Language";
const QString NAME = "Name";
const QString LEVEL = "Level";
const QString COUNT = "Count";
const QString CHECKED = "Checked";
const QString CELLS = "Cells";
const QString CELL = "Cell";
const QString GEAR = "Gear";
const QString SEARCHED_SKILLS = "Searched Skills";
const QString FILTER = "Filter";
const QString SETS = "Sets";
const QString WEAPON_TYPE = "Weapon type";
const QString HEAD = "Head";
const QString TORSO = "Torso";
const QString ARMS = "Arms";
const QString LEGS = "Legs";
const QString WEAPON = "Weapon";
const QString CELL1 = "Cell 1";
const QString CELL2 = "Cell 2";
const QString LANTERN = "Lantern Cell";
const QString LAST_SEARCH_SAVE_LOCATION = "Last search save location";
const QString LAST_CELL_SAVE_LOCATION = "Last cell save location";
const QString CELL_USAGE = "Cell usage";
const QString LOWER_TIER = "Lower tier";
const QString TIER = "Tier";
const QString HEROIC = "Heroic";

Options::Options()
{
    util::files::createDirIfNotExist(QFileInfo(CONFIG_FILE_NAME).path());
    util::files::createDirIfNotExist(QFileInfo(LAST_SEARCH).path());
    util::files::createDirIfNotExist(QFileInfo(MY_CELLS).path());
    util::files::createDirIfNotExist(STANDARD_SEARCH_SAVE_LOCATION);
}

void Options::loadConfiguration(const Gear::Armoury &armoury, const QString &fileName)
{
    QFile config(fileName);
    if (!config.open(QIODevice::ReadOnly))
    {
        // leave presets
        std::cout << "Couldn't open config file: " << fileName.toStdString() << std::endl;
        return;
    }
    QByteArray configData = config.readAll();
    QJsonDocument jsonDoc(QJsonDocument::fromJson(configData));

    auto json = jsonDoc.object();
    int version = 1;
    if (json.contains(VERSION) && json[VERSION].isDouble())
        version = json[VERSION].toInt();
    if (json.contains(MAX_RESULTS) && json[MAX_RESULTS].isDouble())
        numberOfResults = json[MAX_RESULTS].toInt();
    if (json.contains(LANGUAGE) && json[LANGUAGE].isString())
        language = json[LANGUAGE].toString().toStdString();

    // moved to other file in version 2
    if (version < 2 && json.contains(CELLS) && json[CELLS].isArray())
    {
        for (const auto &cell : json[CELLS].toArray())
        {
            if (!cell.isObject())
                continue;
            auto object = cell.toObject();
            // safe guard
            if (!(object.contains(NAME) && object[NAME].isString() && object.contains(LEVEL) &&
                  object[LEVEL].isDouble() && object.contains(COUNT) && object[COUNT].isDouble()))
                continue;
            cells[Gear::Cell(
                Gear::Skill(object[NAME].toString().toStdString(), object[LEVEL].toInt()),
                armoury.getSkillTypeFor(object[NAME].toString().toStdString()))] =
                object[COUNT].toInt();
        }
    }
    // moved to other file in version 2
    if (version < 2 && json.contains(GEAR) && json[GEAR].isArray())
    {
        for (const auto &gear : json[GEAR].toArray())
        {
            if (!gear.isObject())
                continue;
            auto object = gear.toObject();
            // safe guard
            if (!(object.contains(NAME) && object[NAME].isString() && object.contains(CHECKED) &&
                  object[CHECKED].isBool()))
                continue;
            checkedGear[object[NAME].toString().toStdString()] = object[CHECKED].toBool();
        }
    }
    if (json.contains(LAST_SEARCH_SAVE_LOCATION) && json[LAST_SEARCH_SAVE_LOCATION].isString())
        lastSearchSaveLocation = json[LAST_SEARCH_SAVE_LOCATION].toString();
    if (json.contains(LAST_CELL_SAVE_LOCATION) && json[LAST_CELL_SAVE_LOCATION].isString())
        lastCellSaveLocation = json[LAST_CELL_SAVE_LOCATION].toString();
    if (json.contains(CELL_USAGE) && json[CELL_USAGE].isDouble())
        cellUsage = json[CELL_USAGE].toInt();
    if (json.contains(LOWER_TIER) && json[LOWER_TIER].isBool())
        useLowerTierArmour = json[LOWER_TIER].toBool();
    if (json.contains(TIER) && json[TIER].isDouble())
        tier = json[TIER].toInt();
}

void Options::saveConfiguration(const QString &fileName)
{
    QFile config(fileName);

    if (!config.open(QIODevice::WriteOnly))
    {
        std::cout << "Couldn't open config file: " << fileName.toStdString() << std::endl;
        return;
    }

    QJsonObject json;
    json[VERSION] = CURRENT_VERSION;
    json[MAX_RESULTS] = numberOfResults;
    json[LANGUAGE] = QString::fromStdString(language);
    json[LAST_SEARCH_SAVE_LOCATION] = lastSearchSaveLocation;
    json[LAST_CELL_SAVE_LOCATION] = lastCellSaveLocation;
    json[CELL_USAGE] = cellUsage;
    json[LOWER_TIER] = useLowerTierArmour;
    json[TIER] = tier;
    QJsonDocument jsonDoc(json);
    config.write(jsonDoc.toJson());
}

Gear::Cell Options::jsonToCell(const QJsonObject &json, const Gear::Armoury &armoury,
                               const Dictionary &dict, bool useDict)
{
    // safe guard
    if (!(json.contains(NAME) && json[NAME].isString() && json.contains(LEVEL) &&
          json[LEVEL].isDouble()))
        throw std::exception();
    auto name = json[NAME].toString().toStdString();
    if (useDict)
        name = dict.getTranslationFor(name);
    return Gear::Cell(Gear::Skill(name, json[LEVEL].toInt()), armoury.getSkillTypeFor(name));
}

QJsonObject Options::cellToJson(const Gear::Cell &cell)
{
    QJsonObject jsonCell;
    jsonCell[NAME] = QString::fromStdString(cell.getSkillName());
    jsonCell[LEVEL] = cell.getSkill().getSkillPoints();
    return jsonCell;
}

void Options::save()
{
    try
    {
        saveConfiguration();
        saveSearch();
        saveCells(); // TODO: move cells to different window apart from advanced search and then
                     // only there saving allowed
    }
    catch (OptionsIoException)
    {
    }
}

void Options::saveCells(const QString &fileName)
{
    QFile myCells(fileName);

    if (!myCells.open(QIODevice::WriteOnly))
    {
        std::stringstream ss;
        ss << "Couldn't open file \"" << fileName.toStdString() << "\"";
        std::cout << ss.str() << std::endl;
        throw OptionsIoException(ss.str());
    }
    QJsonObject json;
    json[VERSION] = CURRENT_VERSION;

    QJsonArray cells;
    for (const auto &cell : this->cells)
    {

        QJsonObject jsonCell;
        jsonCell[COUNT] = cell.second;
        jsonCell[CELL] = cellToJson(cell.first);
        cells.push_back(jsonCell);
    }
    json[CELLS] = cells;

    QJsonDocument jsonDoc(json);
    myCells.write(jsonDoc.toJson());
}

const std::vector<util::json::JsonParameter> myCellsParameters = {
    {VERSION, QJsonValue::Type::Double}, {CELLS, QJsonValue::Type::Array}};

void Options::loadCells(const Gear::Armoury &armoury, const Dictionary &dict,
                        const QString &fileName)
{
    QFile myCells(fileName);
    if (!myCells.open(QIODevice::ReadOnly))
    {
        std::stringstream ss;
        ss << "Couldn't open file \"" << fileName.toStdString() << "\"";
        std::cout << ss.str() << std::endl;
        throw OptionsIoException(ss.str());
    }
    QByteArray cellData = myCells.readAll();
    QJsonDocument jsonDoc(QJsonDocument::fromJson(cellData));
    if (!jsonDoc.isObject() && !jsonDoc.isArray())
        throw OptionsIoException("File has unexpected format");
    int version = 1;
    QJsonArray array;
    if (jsonDoc.isObject())
    {
        auto json = jsonDoc.object();
        if (!util::json::parameterCheck(json, myCellsParameters))
            throw OptionsIoException("File has unexpected format");
        version = json[VERSION].toInt();
        array = json[CELLS].toArray();
    }
    else
        array = jsonDoc.array();
    for (const auto &cell : array)
    {
        if (!cell.isObject())
        {
            std::cout << "loadCells: non object cell entry" << std::endl;
            continue;
        }
        auto object = cell.toObject();
        // safe guard
        if (!(object.contains(COUNT) && object[COUNT].isDouble() && object.contains(CELL) &&
              object[CELL].isObject()))
        {
            std::cout << "loadCells: non conform cell entry" << std::endl;
            continue;
        }
        try
        {
            cells[jsonToCell(object[CELL].toObject(), armoury, dict, version < 4)] =
                object[COUNT].toInt();
        }
        catch (...)
        {
            std::cout << "loadCells: couldn't load cell" << std::endl;
        }
    }
}

void Options::saveSearch(const QString &fileName)
{
    QFile search(fileName);

    if (!search.open(QIODevice::WriteOnly))
    {
        std::stringstream ss;
        ss << "Couldn't open file \"" << fileName.toStdString() << "\"";
        std::cout << ss.str() << std::endl;
        throw OptionsIoException(ss.str());
    }

    QJsonObject json;
    json[VERSION] = CURRENT_VERSION;
    json[WEAPON_TYPE] = weaponType;
    QJsonArray gearParts, skills, sets;
    for (const auto &search : skillSearches)
    {
        QJsonObject jsonSearch;
        jsonSearch[FILTER] = search.filter;
        jsonSearch[NAME] = QString::fromStdString(search.skillName);
        jsonSearch[LEVEL] = search.skillLevel;
        skills.push_back(jsonSearch);
    }
    json[SEARCHED_SKILLS] = skills;
    for (const auto &gear : checkedGear)
    {
        QJsonObject jsonGear;
        jsonGear[NAME] = QString::fromStdString(gear.first);
        jsonGear[CHECKED] = gear.second;
        gearParts.push_back(jsonGear);
    }
    json[GEAR] = gearParts;
    for (const auto &set : armourSets)
    {
        QJsonObject jsonSet;
        jsonSet[WEAPON] = weaponToJson(set.getWeapon());
        jsonSet[HEAD] = armourToJson(set.getHead());
        jsonSet[TORSO] = armourToJson(set.getTorso());
        jsonSet[ARMS] = armourToJson(set.getArms());
        jsonSet[LEGS] = armourToJson(set.getLegs());
        if (!set.getLantern().isEmpty())
            jsonSet[LANTERN] = cellToJson(set.getLantern());
        sets.push_back(jsonSet);
    }
    json[SETS] = sets;

    QJsonDocument jsonDoc(json);
    search.write(jsonDoc.toJson());
}

const std::vector<util::json::JsonParameter> searchParameters = {
    {WEAPON_TYPE, QJsonValue::Type::Double},
    {SEARCHED_SKILLS, QJsonValue::Type::Array},
    {GEAR, QJsonValue::Type::Array},
    {SETS, QJsonValue::Type::Array}};

void Options::loadSearch(const Gear::Armoury &armoury, const Dictionary &dict,
                         const QString &fileName)
{
    QFile search(fileName);
    if (!search.open(QIODevice::ReadOnly))
    {
        std::stringstream ss;
        ss << "Couldn't open file \"" << fileName.toStdString() << "\"";
        std::cout << ss.str() << std::endl;
        throw OptionsIoException(ss.str());
    }
    QByteArray searchData = search.readAll();
    QJsonDocument jsonDoc(QJsonDocument::fromJson(searchData));
    if (!jsonDoc.isObject())
        throw OptionsIoException("File has unexpected format");
    auto json = jsonDoc.object();
    if (!util::json::parameterCheck(json, searchParameters))
        throw OptionsIoException("File has unexpected format");
    int version = 1;
    if (json.contains(VERSION) && json[VERSION].isDouble())
        version = json[VERSION].toInt();
    weaponType = (Gear::WeaponType)json[WEAPON_TYPE].toInt();
    if (json.contains(SEARCHED_SKILLS) && json[SEARCHED_SKILLS].isArray())
    {
        size_t i = 0;
        for (const auto &skill : json[SEARCHED_SKILLS].toArray())
        {
            if (!skill.isObject())
                continue;
            auto json = skill.toObject();
            SkillSearch search;
            // safeguard
            if (!(json.contains(FILTER) && json[FILTER].isDouble() && json.contains(NAME) &&
                  json[NAME].isString() && json.contains(LEVEL) && json[LEVEL].isDouble()))
            {
                std::cout << "filter " << i << " is not conform" << std::endl;
                continue;
            }
            search.filter = (Gear::SkillType)json[FILTER].toInt();
            auto name = json[NAME].toString().toStdString();
            if (version < 4 && !name.empty())
                name = dict.getTranslationFor(name);
            search.skillName = name;
            search.skillLevel = json[LEVEL].toInt();
            skillSearches[i] = search;
            ++i;
        }
    }
    if (json.contains(GEAR) && json[GEAR].isArray())
    {
        for (const auto &gear : json[GEAR].toArray())
        {
            if (!gear.isObject())
                continue;
            auto json = gear.toObject();
            if (!(json.contains(NAME) && json[NAME].isString() && json.contains(CHECKED) &&
                  json[CHECKED].isBool()))
            {
                std::cout << "checked gear is not conform" << std::endl;
                continue;
            }
            auto name = json[NAME].toString().toStdString();
            if (version < 4)
                name = dict.getTranslationFor(name);
            checkedGear[name] = json[CHECKED].toBool();
        }
    }
    armourSets.clear();
    if (json.contains(SETS) && json[SETS].isArray())
    {
        for (auto const &set : json[SETS].toArray())
        {
            if (!set.isObject())
                continue;
            auto json = set.toObject();
            if (!(json.contains(WEAPON) && json[WEAPON].isObject() && json.contains(HEAD) &&
                  json[HEAD].isObject() && json.contains(TORSO) && json[TORSO].isObject() &&
                  json.contains(ARMS) && json[ARMS].isObject() && json.contains(LEGS) &&
                  json[LEGS].isObject()))
                continue;
            try
            {
                auto weapon = jsonToWeapon(json[WEAPON].toObject(), armoury, dict, version < 4);
                auto head = jsonToArmour(json[HEAD].toObject(), armoury, dict, version < 4);
                auto torso = jsonToArmour(json[TORSO].toObject(), armoury, dict, version < 4);
                auto arms = jsonToArmour(json[ARMS].toObject(), armoury, dict, version < 4);
                auto legs = jsonToArmour(json[LEGS].toObject(), armoury, dict, version < 4);
                Gear::ArmourSet set(head, torso, arms, legs, weapon);
                if (json.contains(LANTERN) && json[LANTERN].isObject())
                    if (!set.addCell(
                            jsonToCell(json[LANTERN].toObject(), armoury, dict, version < 4)))
                    {
                        std::cout << "lantern cell does not fit" << std::endl;
                        continue;
                    }
                armourSets.push_back(set);
            }
            catch (const std::exception &e)
            {
                std::cout << e.what() << std::endl;
            }
        }
    }
}

QJsonObject Options::armourToJson(const Gear::Armour &armour)
{
    QJsonObject json;
    json[NAME] = QString::fromStdString(armour.getName());
    if (armour.getTiers().size() == 1)
    {
        if (armour.getTiers()[0] == 6)
            json[HEROIC] = true;
    }
    if (!armour.getCell().isEmpty())
        json[CELL] = cellToJson(armour.getCell());
    return json;
}

Gear::Armour Options::jsonToArmour(const QJsonObject &json, const Gear::Armoury &armoury,
                                   const Dictionary &dict, bool useDict)
{
    // safeguard
    if (!(json.contains(NAME) && json[NAME].isString()))
        throw std::exception("Armour has no name");
    bool heroic = false;
    if (json.contains(HEROIC) && json[HEROIC].isBool())
        heroic = json[HEROIC].toBool();
    auto name = json[NAME].toString().toStdString();
    if (useDict)
    {
        name = dict.getTranslationFor(name);
        heroic = true;
    }
    auto armour = armoury.getArmour(name, heroic);
    if (json.contains(CELL) && json[CELL].isObject())
        if (!armour.addCell(jsonToCell(json[CELL].toObject(), armoury, dict, useDict)))
            throw std::runtime_error("Armour " + name + " cell does not fit");
    return armour;
}

QJsonObject Options::weaponToJson(const Gear::Weapon &weapon)
{
    QJsonObject json;
    json[NAME] = QString::fromStdString(weapon.getName());
    if (weapon.getTiers().size() == 1)
    {
        if (weapon.getTiers()[0] == 6)
            json[HEROIC] = true;
    }
    if (!weapon.getCell1().isEmpty())
        json[CELL1] = cellToJson(weapon.getCell1());
    if (!weapon.getCell2().isEmpty())
        json[CELL2] = cellToJson(weapon.getCell2());
    return json;
}

Gear::Weapon Options::jsonToWeapon(const QJsonObject &json, const Gear::Armoury &armoury,
                                   const Dictionary &dict, bool useDict)
{
    // safequard
    if (!(json.contains(NAME) && json[NAME].isString()))
        throw std::exception("Weapon has no name");
    bool heroic = false;
    if (json.contains(HEROIC) && json[HEROIC].isBool())
        heroic = json[HEROIC].toBool();
    auto name = json[NAME].toString().toStdString();
    if (useDict)
    {
        name = dict.getTranslationFor(name);
        heroic = true;
    }
    auto weapon = armoury.getWeapon(name, heroic);
    if (json.contains(CELL1) && json[CELL1].isObject())
        if (!weapon.addCell(jsonToCell(json[CELL1].toObject(), armoury, dict, useDict)))
            throw std::runtime_error("Weapon " + name + " cell 1 does not fit");
    if (json.contains(CELL2) && json[CELL2].isObject())
        if (!weapon.addCell(jsonToCell(json[CELL2].toObject(), armoury, dict, useDict)))
            throw std::runtime_error("Weapon " + name + " cell 2 does not fit");
    return weapon;
}
