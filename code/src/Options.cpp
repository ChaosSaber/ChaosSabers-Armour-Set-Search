#include "Options.hpp"
#include "util/json.hpp"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <iostream>
#include <sstream>

#define CURRENT_VERSION 5

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
const QString WEAPON_ELEMENT = "Weapon element";
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

void Options::loadConfiguration(const Gear::Armoury& armoury, const QString& fileName)
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
        for (const auto& cell : json[CELLS].toArray())
        {
            if (!cell.isObject())
                continue;
            auto object = cell.toObject();
            // safe guard
            if (!(object.contains(NAME) && object[NAME].isString() && object.contains(LEVEL) &&
                  object[LEVEL].isDouble() && object.contains(COUNT) && object[COUNT].isDouble()))
                continue;
            auto skillId = armoury.getSkillIdForName(object[NAME].toString().toStdString());
            cells[Gear::Cell(Gear::Skill(skillId, object[LEVEL].toInt()),
                             armoury.getSkillTypeFor(skillId))] = object[COUNT].toInt();
        }
    }
    // moved to other file in version 2
    if (version < 2 && json.contains(GEAR) && json[GEAR].isArray())
    {
        for (const auto& gear : json[GEAR].toArray())
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
    if (version < 5)
    {
        if (tier < 5)
            tier = 1;
        else if (tier == 5)
            tier = 2;
        else if (tier == 6)
            tier = 3;
    }
}

void Options::saveConfiguration(const QString& fileName)
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

QJsonArray Options::cellListToJson(const Gear::CellList& cells, const Gear::Armoury& armoury)
{
    QJsonArray jsonCells;
    for (auto& cell : cells)
    {
        if (cell.first.isEmpty())
            continue;
        for (size_t count = 0; count < cell.second; ++count)
            jsonCells.push_back(cellToJson(cell.first, armoury));
    }
    return jsonCells;
}

Gear::CellList Options::jsonToCellList(const QJsonArray& json, const Gear::Armoury& armoury)
{
    Gear::CellList cells;
    for (auto& cell : json)
    {
        if (!cell.isObject())
            throw std::exception("cell is no object");
        cells += jsonToCell(cell.toObject(), armoury);
    }
    return cells;
}

Gear::Cell Options::jsonToCell(const QJsonObject& json, const Gear::Armoury& armoury)
{
    // safe guard
    if (!(json.contains(NAME) && json[NAME].isString() && json.contains(LEVEL) &&
          json[LEVEL].isDouble()))
        throw std::exception();
    auto skillId = armoury.getSkillIdForName(json[NAME].toString().toStdString());
    return Gear::Cell(Gear::Skill(skillId, json[LEVEL].toInt()), armoury.getSkillTypeFor(skillId));
}

QJsonObject Options::cellToJson(const Gear::Cell& cell, const Gear::Armoury& armoury)
{
    QJsonObject jsonCell;
    jsonCell[NAME] = QString::fromStdString(armoury.getSkillInfo(cell.getSkillId()).getName());
    jsonCell[LEVEL] = (int)cell.getSkill().getSkillPoints();
    return jsonCell;
}

void Options::save(const Gear::Armoury& armoury)
{
    try
    {
        saveConfiguration();
        saveSearch(armoury);
        saveCells(armoury); // TODO: move cells to different window apart from advanced search and
                            // then only there saving allowed
    }
    catch (OptionsIoException)
    {
    }
}

void Options::saveCells(const Gear::Armoury& armoury, const QString& fileName)
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
    for (const auto& cell : this->cells)
    {

        QJsonObject jsonCell;
        jsonCell[COUNT] = cell.second;
        jsonCell[CELL] = cellToJson(cell.first, armoury);
        cells.push_back(jsonCell);
    }
    json[CELLS] = cells;

    QJsonDocument jsonDoc(json);
    myCells.write(jsonDoc.toJson());
}

const std::vector<util::json::JsonParameter> myCellsParameters = {
    {VERSION, QJsonValue::Type::Double}, {CELLS, QJsonValue::Type::Array}};

void Options::loadCells(const Gear::Armoury& armoury, const Dictionary& dict,
                        const QString& fileName)
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
    for (const auto& cell : array)
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
            cells[jsonToCell(object[CELL].toObject(), armoury)] = object[COUNT].toInt();
        }
        catch (...)
        {
            std::cout << "loadCells: couldn't load cell" << std::endl;
        }
    }
}

void Options::saveSearch(const Gear::Armoury& armoury, const QString& fileName)
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
    json[WEAPON_ELEMENT] = weaponElement;
    QJsonArray gearParts, skills, sets;
    for (const auto& search : skillSearches)
    {
        QJsonObject jsonSearch;
        jsonSearch[FILTER] = search.filter;
        jsonSearch[NAME] = QString::fromStdString(search.skillName);
        jsonSearch[LEVEL] = search.skillLevel;
        skills.push_back(jsonSearch);
    }
    json[SEARCHED_SKILLS] = skills;
    for (const auto& gear : checkedGear)
    {
        QJsonObject jsonGear;
        jsonGear[NAME] = QString::fromStdString(gear.first);
        jsonGear[CHECKED] = gear.second;
        gearParts.push_back(jsonGear);
    }
    json[GEAR] = gearParts;
    for (const auto& set : armourSets)
    {
        QJsonObject jsonSet;
        jsonSet[WEAPON] = gearToJson(set.getWeapon());
        jsonSet[HEAD] = gearToJson(set.getHead());
        jsonSet[TORSO] = gearToJson(set.getTorso());
        jsonSet[ARMS] = gearToJson(set.getArms());
        jsonSet[LEGS] = gearToJson(set.getLegs());
        jsonSet[CELLS] = cellListToJson(set.getCellList(), armoury);
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

const std::vector<util::json::JsonParameter> armourSetParameters = {
    {WEAPON, QJsonValue::Type::Object}, {HEAD, QJsonValue::Type::Object},
    {TORSO, QJsonValue::Type::Object},  {ARMS, QJsonValue::Type::Object},
    {LEGS, QJsonValue::Type::Object},   {CELLS, QJsonValue::Type::Array}};

void Options::loadSearch(const Gear::Armoury& armoury, const Dictionary& dict,
                         const QString& fileName)
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
    if (json.contains(WEAPON_ELEMENT) && json[WEAPON_ELEMENT].isDouble())
        weaponElement = (Gear::Element)json[WEAPON_ELEMENT].toInt();
    if (json.contains(SEARCHED_SKILLS) && json[SEARCHED_SKILLS].isArray())
    {
        size_t i = 0;
        for (const auto& skill : json[SEARCHED_SKILLS].toArray())
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

    // complete Armour rework in OB 0.7.1 therefore no loading of old armours
    if (version < 5)
        return;
    if (json.contains(GEAR) && json[GEAR].isArray())
    {
        for (const auto& gear : json[GEAR].toArray())
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
            checkedGear[name] = json[CHECKED].toBool();
        }
    }
    armourSets.clear();
    if (json.contains(SETS) && json[SETS].isArray())
    {
        for (auto const& set : json[SETS].toArray())
        {
            if (!set.isObject())
                continue;
            auto json = set.toObject();
            if (!util::json::parameterCheck(json, armourSetParameters))
                continue;
            try
            {
                auto weapon = jsonToWeapon(json[WEAPON].toObject(), armoury);
                auto head = jsonToArmour(json[HEAD].toObject(), armoury);
                auto torso = jsonToArmour(json[TORSO].toObject(), armoury);
                auto arms = jsonToArmour(json[ARMS].toObject(), armoury);
                auto legs = jsonToArmour(json[LEGS].toObject(), armoury);
                Gear::ArmourSet set(armoury, std::move(head), std::move(torso), std::move(arms),
                                    std::move(legs), std::move(weapon));
                for (auto& cell : jsonToCellList(json[CELLS].toArray(), armoury))
                    for (size_t count = 0; count < cell.second; ++count)
                        if (!set.addCell(cell.first))
                        {
                            std::cout << "cells do not fit into armour set" << std::endl;
                            continue;
                        }
                armourSets.push_back(std::move(set));
            }
            catch (const std::exception& e)
            {
                std::cout << e.what() << std::endl;
            }
        }
    }
}

QJsonObject Options::gearToJson(const Gear::Gear& armour)
{
    QJsonObject json;
    json[NAME] = QString::fromStdString(armour.getName());
    json[LEVEL] = armour.getLevel();
    return json;
}

const std::vector<util::json::JsonParameter> gearParameters = {{NAME, QJsonValue::Type::String},
                                                               {LEVEL, QJsonValue::Type::Double}};

std::tuple<std::string, int> Options::jsonToGear(const QJsonObject& json)
{
    if (!util::json::parameterCheck(json, gearParameters))
        throw std::exception("Gear is not conform");
    auto name = json[NAME].toString().toStdString();
    auto level = json[LEVEL].toInt();
    return {name, level};
}

Gear::Armour Options::jsonToArmour(const QJsonObject& json, const Gear::Armoury& armoury)
{
    auto [name, level] = jsonToGear(json);
    auto armour = armoury.getArmour(name, level);
    return armour;
}

Gear::Weapon Options::jsonToWeapon(const QJsonObject& json, const Gear::Armoury& armoury)
{
    auto [name, level] = jsonToGear(json);
    auto weapon = armoury.getWeapon(name, level);
    return weapon;
}
