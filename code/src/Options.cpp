#include "Options.hpp"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <iostream>

const QString MAX_RESULTS = "Results";
const QString LANGUAGE = "Language";
const QString NAME = "Name";
const QString LEVEL = "Level";
const QString COUNT = "Count";
const QString CHECKED = "Checked";
const QString CELLS = "Cells";
const QString GEAR = "Gear";

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
    json[MAX_RESULTS] = numberOfResults;
    json[LANGUAGE] = QString::fromStdString(language);

    QJsonArray cells, gearParts;
    for (const auto &cell : this->cells)
    {
        QJsonObject jsonCell;
        jsonCell[NAME] = QString::fromStdString(cell.first.getSkillName());
        jsonCell[LEVEL] = cell.first.getSkill().getSkillPoints();
        jsonCell[COUNT] = cell.second;
        cells.push_back(jsonCell);
    }
    json[CELLS] = cells;

    for (const auto &gear : checkedGear)
    {
        QJsonObject jsonGear;
        jsonGear[NAME] = QString::fromStdString(gear.first);
        jsonGear[CHECKED] = gear.second;
        gearParts.push_back(jsonGear);
    }
    json[GEAR] = gearParts;

    QJsonDocument jsonDoc(json);
    config.write(jsonDoc.toJson());
}