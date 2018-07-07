#include "Options.hpp"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <iostream>

#define CURRENT_VERSION 2

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
const QString WEAPON_TYPE = "Weapon type";
const QString HEAD = "Head";
const QString TORSO = "Torso";
const QString ARMS = "Arms";
const QString LEGS = "Legs";
const QString WEAPON = "Weapon";
const QString CELL1 = "Cell 1";
const QString CELL2 = "Cell 2";

void Options::load(const Gear::Armoury &armoury, const std::string &fileName)
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
    int version = 1;
    if (json.contains(VERSION) && json[VERSION].isDouble())
        version = json[VERSION].toInt();
    if (json.contains(MAX_RESULTS) && json[MAX_RESULTS].isDouble())
        numberOfResults = json[MAX_RESULTS].toInt();
    if (json.contains(LANGUAGE) && json[LANGUAGE].isString())
        language = json[LANGUAGE].toString().toStdString();

    if (json.contains(CELLS) && json[CELLS].isArray())
    {
        for (const auto &cell : json[CELLS].toArray())
        {
            if (!cell.isObject())
                continue;
            auto object = cell.toObject();
            if (version < 2)
            {
                // safe guard
                if (!(object.contains(NAME) && object[NAME].isString() && object.contains(LEVEL) &&
                      object[LEVEL].isDouble() && object.contains(COUNT) &&
                      object[COUNT].isDouble()))
                    continue;
                cells[Gear::Cell(
                    Gear::Skill(object[NAME].toString().toStdString(), object[LEVEL].toInt()),
                    armoury.getSkillTypeFor(object[NAME].toString().toStdString()))] =
                    object[COUNT].toInt();
            }
            else
            {
                // safe guard
                if (!(object.contains(COUNT) && object[COUNT].isDouble() && object.contains(CELL) &&
                      object[CELL].isObject()))
                    continue;
                try
                {
                    cells[loadCell(object[CELL].toObject(), armoury)] = object[COUNT].toInt();
                }
                catch (...)
                {
                }
            }
        }
    }
    if (json.contains(GEAR) && json[GEAR].isArray())
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
}

void Options::save(const std::string &fileName)
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

    QJsonArray cells;
    for (const auto &cell : this->cells)
    {

        QJsonObject jsonCell;
        jsonCell[COUNT] = cell.second;
        jsonCell[CELL] = saveCell(cell.first);
        cells.push_back(jsonCell);
    }
    json[CELLS] = cells;

    QJsonDocument jsonDoc(json);
    config.write(jsonDoc.toJson());
}

void Options::saveSearch(const std::string &fileName)
{
    QFile config(QString::fromStdString(fileName));

    if (!config.open(QIODevice::WriteOnly))
    {
        std::cout << "Couldn't open config file";
        return;
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
    for (const auto &set : armourSets)
    {
        QJsonObject jsonSet, weapon, head, torso, arms, legs;
    }
    json[GEAR] = gearParts;
}

Gear::Cell Options::loadCell(const QJsonObject &json, const Gear::Armoury &armoury)
{
    // safe guard
    if (!(json.contains(NAME) && json[NAME].isString() && json.contains(LEVEL) &&
          json[LEVEL].isDouble()))
        throw std::exception();
    return Gear::Cell(Gear::Skill(json[NAME].toString().toStdString(), json[LEVEL].toInt()),
                      armoury.getSkillTypeFor(json[NAME].toString().toStdString()));
}

QJsonObject Options::saveCell(const Gear::Cell &cell)
{
    QJsonObject jsonCell;
    jsonCell[NAME] = QString::fromStdString(cell.getSkillName());
    jsonCell[LEVEL] = cell.getSkill().getSkillPoints();
    return jsonCell;
}
