#include "Options.hpp"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <iostream>
#include <sstream>

#define CURRENT_VERSION 3

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
const QString LAST_SAVE_LOCATION = "Last save location";
const QString CELL_USAGE = "Cell usage";

void Options::loadConfiguration(const Gear::Armoury &armoury, const std::string &fileName)
{
    QFile config(QString::fromStdString(fileName));
    if (!config.open(QIODevice::ReadOnly))
    {
        // leave presets
        std::cout << "Couldn't open config file";
        return;
    }
    QByteArray configData = config.readAll();
    QJsonDocument jsonDoc(QJsonDocument::fromJson(configData));

    auto json = jsonDoc.object();
    version = 1;
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
    if (json.contains(LAST_SAVE_LOCATION) && json[LAST_SAVE_LOCATION].isString())
        lastSaveLocation = json[LAST_SAVE_LOCATION].toString();
    if (json.contains(CELL_USAGE) && json[CELL_USAGE].isDouble())
        cellUsage = json[CELL_USAGE].toInt();
}

void Options::saveConfiguration(const std::string &fileName)
{
    QFile config(QString::fromStdString(fileName));

    if (!config.open(QIODevice::WriteOnly))
    {
        std::cout << "Couldn't open config file";
        return;
    }

    QJsonObject json;
    json[VERSION] = CURRENT_VERSION;
    json[MAX_RESULTS] = numberOfResults;
    json[LANGUAGE] = QString::fromStdString(language);
    json[LAST_SAVE_LOCATION] = lastSaveLocation;
    json[CELL_USAGE] = cellUsage;
    QJsonDocument jsonDoc(json);
    config.write(jsonDoc.toJson());
}

Gear::Cell Options::jsonToCell(const QJsonObject &json, const Gear::Armoury &armoury)
{
    // safe guard
    if (!(json.contains(NAME) && json[NAME].isString() && json.contains(LEVEL) &&
          json[LEVEL].isDouble()))
        throw std::exception();
    return Gear::Cell(Gear::Skill(json[NAME].toString().toStdString(), json[LEVEL].toInt()),
                      armoury.getSkillTypeFor(json[NAME].toString().toStdString()));
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

void Options::load(const Gear::Armoury &armoury)
{
    try
    {
        loadConfiguration(armoury);
        loadSearch(armoury);
        loadCells(armoury);
    }
    catch (OptionsIoException)
    {
    }
}

void Options::saveCells(const std::string &fileName)
{
    QFile myCells(QString::fromStdString(fileName));

    if (!myCells.open(QIODevice::WriteOnly))
    {
        std::stringstream ss;
        ss << "Couldn't open file \"" << fileName << "\"";
        std::cout << ss.str();
        throw OptionsIoException(ss.str());
    }

    QJsonArray cells;
    for (const auto &cell : this->cells)
    {

        QJsonObject jsonCell;
        jsonCell[COUNT] = cell.second;
        jsonCell[CELL] = cellToJson(cell.first);
        cells.push_back(jsonCell);
    }

    QJsonDocument jsonDoc(cells);
    myCells.write(jsonDoc.toJson());
}

void Options::loadCells(const Gear::Armoury &armoury, std::string fileName)
{
    if (version == 2 && fileName == MY_CELLS)
        fileName = MY_CELLS_OLD;
    QFile myCells(QString::fromStdString(fileName));
    if (!myCells.open(QIODevice::ReadOnly))
    {
        std::stringstream ss;
        ss << "Couldn't open file \"" << fileName << "\"";
        std::cout << ss.str();
        throw OptionsIoException(ss.str());
    }
    QByteArray cellData = myCells.readAll();
    QJsonDocument jsonDoc(QJsonDocument::fromJson(cellData));
    if (!jsonDoc.isArray())
        throw OptionsIoException("File has unexpected format");
    for (const auto &cell : jsonDoc.array())
    {
        if (!cell.isObject())
            continue;
        auto object = cell.toObject();
        // safe guard
        if (!(object.contains(COUNT) && object[COUNT].isDouble() && object.contains(CELL) &&
              object[CELL].isObject()))
            continue;
        try
        {
            cells[jsonToCell(object[CELL].toObject(), armoury)] = object[COUNT].toInt();
        }
        catch (...)
        {
        }
    }
}

void Options::saveSearch(const std::string &fileName)
{
    QFile search(QString::fromStdString(fileName));

    if (!search.open(QIODevice::WriteOnly))
    {
        std::stringstream ss;
        ss << "Couldn't open file \"" << fileName << "\"";
        std::cout << ss.str();
        throw OptionsIoException(ss.str());
    }

    QJsonObject json;
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

void Options::loadSearch(const Gear::Armoury &armoury, const std::string &fileName)
{
    QFile search(QString::fromStdString(fileName));
    if (!search.open(QIODevice::ReadOnly))
    {
        std::stringstream ss;
        ss << "Couldn't open file \"" << fileName << "\"";
        std::cout << ss.str();
        throw OptionsIoException(ss.str());
    }
    QByteArray searchData = search.readAll();
    QJsonDocument jsonDoc(QJsonDocument::fromJson(searchData));
    if (!jsonDoc.isObject())
        throw OptionsIoException("File has unexpected format");
    auto json = jsonDoc.object();
    if (json.contains(WEAPON_TYPE) && json[WEAPON_TYPE].isDouble())
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
                continue;
            search.filter = (Gear::SkillType)json[FILTER].toInt();
            search.skillName = json[NAME].toString().toStdString();
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
                continue;
            checkedGear[json[NAME].toString().toStdString()] = json[CHECKED].toBool();
        }
    }
    if (json.contains(SETS) && json[SETS].isArray())
    {
        armourSets.clear();
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
                auto weapon = jsonToWeapon(json[WEAPON].toObject(), armoury);
                auto head = jsonToArmour(json[HEAD].toObject(), armoury);
                auto torso = jsonToArmour(json[TORSO].toObject(), armoury);
                auto arms = jsonToArmour(json[ARMS].toObject(), armoury);
                auto legs = jsonToArmour(json[LEGS].toObject(), armoury);
                Gear::ArmourSet set(head, torso, arms, legs, weapon);
                if (json.contains(LANTERN) && json[LANTERN].isObject())
                    if (!set.addCell(jsonToCell(json[LANTERN].toObject(), armoury)))
                        continue;
                armourSets.push_back(set);
            }
            catch (std::exception)
            {
                // implement some debug logging?
            }
        }
    }
}

QJsonObject Options::armourToJson(const Gear::Armour &armour)
{
    QJsonObject json;
    json[NAME] = QString::fromStdString(armour.getName());
    if (!armour.getCell().isEmpty())
        json[CELL] = cellToJson(armour.getCell());
    return json;
}

Gear::Armour Options::jsonToArmour(const QJsonObject &json, const Gear::Armoury &armoury)
{
    // safeguard
    if (!(json.contains(NAME) && json[NAME].isString()))
        throw std::exception();
    auto armour = armoury.getArmour(json[NAME].toString().toStdString());
    if (json.contains(CELL) && json[CELL].isObject())
        if (!armour.addCell(jsonToCell(json[CELL].toObject(), armoury)))
            throw std::exception();
    return armour;
}

QJsonObject Options::weaponToJson(const Gear::Weapon &weapon)
{
    QJsonObject json;
    json[NAME] = QString::fromStdString(weapon.getName());
    if (!weapon.getCell1().isEmpty())
        json[CELL1] = cellToJson(weapon.getCell1());
    if (!weapon.getCell2().isEmpty())
        json[CELL2] = cellToJson(weapon.getCell2());
    return json;
}

Gear::Weapon Options::jsonToWeapon(const QJsonObject &json, const Gear::Armoury &armoury)
{
    // safequard
    if (!(json.contains(NAME) && json[NAME].isString()))
        throw std::exception();
    auto weapon = armoury.getWeapon(json[NAME].toString().toStdString());
    if (json.contains(CELL1) && json[CELL1].isObject())
        if (!weapon.addCell(jsonToCell(json[CELL1].toObject(), armoury)))
            throw std::exception();
    if (json.contains(CELL2) && json[CELL2].isObject())
        if (!weapon.addCell(jsonToCell(json[CELL2].toObject(), armoury)))
            throw std::exception();
    return weapon;
}
