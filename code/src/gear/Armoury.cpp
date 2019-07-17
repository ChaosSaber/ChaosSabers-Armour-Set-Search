#include "gear/Armoury.hpp"
#include "Options.hpp"
#include "util/json.hpp"
#include "util/string.hpp"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <fstream>
#include <iostream>

Gear::Armoury::Armoury(Dictionary& dict)
    : notFound("", "", None, std::vector<std::string>()), dict(dict)
{
}

std::vector<Gear::Weapon> Gear::Armoury::getWeaponsWithSkill(const std::vector<Skill>& skills,
                                                             WeaponType type,
                                                             const Options& options) const
{
    std::vector<Weapon> weapons;
    if (this->weapons.count(type) == 0)
        return weapons;
    for (const auto& weapon : this->weapons.at(type))
    {
        if (filterWeapon(weapon, options))
            continue;
        if (options.weaponElement != Element::All)
        {
            weapons.push_back(weapon);
            continue;
        }
        for (const auto& skill : skills)
        {
            if (weapon.getSkillPointsFor(skill.getId()) > 0 ||
                weapon.hasFreeCellSlotFor(getSkillTypeFor(skill.getId())))
            {
                weapons.push_back(weapon);
                break;
            }
        }
    }
    return weapons;
}

std::vector<Gear::Armour> Gear::Armoury::getArmourWithSkill(const std::vector<Skill>& skills,
                                                            ArmourType type,
                                                            const Options& options) const
{
    std::vector<Armour> armours;
    if (this->armours.count(type) == 0)
        return armours;
    for (const auto& armour : this->armours.at(type))
    {
        if (filterGear(armour, options))
            continue;
        for (const auto& skill : skills)
        {
            if (armour.hasSkill(skill.getId()) ||
                armour.hasFreeCellSlotFor(getSkillTypeFor(skill.getId())))
            {
                armours.push_back(armour);
                break;
            }
        }
    }
    // TODO: add generic armour without skill but corresponding cell slot
    // for example any armour with mobility Slot
    return armours;
}

std::vector<const Gear::SkillInfo*> Gear::Armoury::getSkills(SkillType filter) const
{
    std::vector<const SkillInfo*> skills;
    for (const auto& skill : skillInfos)
    {
        if (skill.getName().empty())
            continue;
        if (filter == None || filter == skill.getType())
            skills.push_back(&skill);
    }
    return skills;
}

const Gear::SkillInfo& Gear::Armoury::getSkillInfo(size_t id) const { return skillInfos.at(id); }

Gear::SkillType Gear::Armoury::getSkillTypeFor(size_t id) const
{
    return getSkillInfo(id).getType();
}

const Gear::Armour& Gear::Armoury::getArmour(std::string name, int level) const
{
    for (const auto& armours : this->armours)
        for (const auto& armour : armours.second)
            if (armour.getName() == name && armour.getLevel() == level)
                return armour;
    std::stringstream ss;
    ss << "There is no armour with the key " << name;
    throw std::exception(ss.str().c_str());
}

const Gear::Weapon& Gear::Armoury::getWeapon(std::string name, int level) const
{
    for (const auto& weapons : this->weapons)
        for (const auto& weapon : weapons.second)
            if (weapon.getName() == name && weapon.getLevel() == level)
                return weapon;
    std::stringstream ss;
    ss << "There is no weapon with the key " << name << " and level " << level;
    throw std::exception(ss.str().c_str());
}

#define JSON_ARMOURS "armours"
#define JSON_PERKS "perks"
#define JSON_WEAPONS "weapons"

const std::vector<util::json::JsonParameter> armouryParameters = {
    {JSON_ARMOURS, QJsonValue::Type::Object},
    {JSON_PERKS, QJsonValue::Type::Object},
    {JSON_WEAPONS, QJsonValue::Type::Object}};

#define JSON_TYPE "type"
#define JSON_STRENGTH "strength"
#define JSON_WEAKNESS "weakness"
#define JSON_CELLS "cells"
#define JSON_RESISTANCE "resistance"
#define JSON_PERKS "perks"
#define JSON_UNIQUE_EFFECT "unique_effects"
#define JSON_ELEMENTAL "elemental"
#define JSON_POWER "power"
#define JSON_EFFECTS "effects"

const std::vector<util::json::JsonParameter> armourParameters = {
    {JSON_NAME, QJsonValue::Type::String},
    {JSON_DESCRIPTION, QJsonValue::Type::String},
    {JSON_TYPE, QJsonValue::Type::String},
    {JSON_RESISTANCE, QJsonValue::Type::Object}};

const std::vector<util::json::JsonParameter> weaponParameters = {
    {JSON_NAME, QJsonValue::Type::String},
    {JSON_DESCRIPTION, QJsonValue::Type::String},
    {JSON_TYPE, QJsonValue::Type::String},
    {JSON_POWER, QJsonValue::Type::Object}};

const std::vector<util::json::JsonParameter> skillParameters = {
    {JSON_NAME, QJsonValue::Type::String},
    {JSON_DESCRIPTION, QJsonValue::Type::String},
    {JSON_TYPE, QJsonValue::Type::String},
    {JSON_EFFECTS, QJsonValue::Type::Object}};

size_t Gear::Armoury::getSkillIdForName(const std::string& name) const
{
    return mapSkillNameToId_.at(name);
}

void Gear::Armoury::load(const std::string& fileName)
{
    weapons.clear();
    armours.clear();
    skillInfos.clear();
    mapSkillNameToId_.clear();
    QFile config(QString::fromStdString(fileName));
    if (!config.open(QIODevice::ReadOnly))
    {
        // leave presets
        std::cout << "Couldn't open config file: " << fileName << std::endl;
        return;
    }
    QByteArray configData = config.readAll();
    QJsonDocument jsonDoc(QJsonDocument::fromJson(configData));

    auto json = jsonDoc.object();
    if (!util::json::parameterCheck(json, armouryParameters))
    {
        std::cout << "Error reading data files" << std::endl;
        return;
    }
    skillInfos.push_back(notFound);
    auto jsonPerks = json[JSON_PERKS].toObject();
    for (const auto& key : jsonPerks.keys())
    {
        try
        {
            if (!jsonPerks[key].isObject())
            {
                std::cout << "found non object weapon entry: " << key.toStdString() << std::endl;
                continue;
            }
            auto perk = jsonPerks[key].toObject();
            if (!util::json::parameterCheck(perk, skillParameters))
            {
                std::cout << "found non conforming skill entry: " << key.toStdString() << std::endl;
                continue;
            }
            std::string name = perk[JSON_NAME].toString().toStdString();
            auto type = getSkillType(perk[JSON_TYPE].toString().toStdString());
            std::string description = perk[JSON_DESCRIPTION].toString().toStdString();
            std::vector<std::string> effects;
            auto jsonEffects = perk[JSON_EFFECTS].toObject();
            int i = 1;
            while (jsonEffects.contains(QString::number(i)) &&
                   jsonEffects[QString::number(i)].isObject())
            {
                auto effect = jsonEffects[QString::number(i)].toObject();
                ++i;
                if (!effect.contains(JSON_DESCRIPTION))
                {
                    std::cout << key.toStdString() << ": effect " << i
                              << ": doesn't contain a description" << std::endl;
                    break;
                }
                if (effect[JSON_DESCRIPTION].isString())
                    effects.push_back(effect[JSON_DESCRIPTION].toString().toStdString());
                else if (effect[JSON_DESCRIPTION].isArray())
                {
                    std::vector<std::string> descriptions;
                    auto arr = effect[JSON_DESCRIPTION].toArray();
                    for (int i = 0; i < arr.size(); ++i)
                    {
                        if (arr[i].isNull())
                            continue;
                        if (arr[i].isString())
                            descriptions.push_back(arr[i].toString().toStdString());
                        else
                        {
                            std::cout << key.toStdString() << ": effect " << i
                                      << ": unknown description type: " << arr[i].type()
                                      << std::endl;
                            break;
                        }
                    }
                    auto description = util::string::vectorJoin(descriptions, ". ");
                    dict.addEntry(description, description);
                    effects.push_back(description);
                }
                else
                {
                    std::cout << key.toStdString() << ": effect " << i
                              << ": unknown type: " << effect[JSON_DESCRIPTION].type() << std::endl;
                }
            }
            skillInfos.push_back(SkillInfo(name, description, type, effects));
            dict.addEntry(name, name);
            dict.addEntry(description, description);
        }
        catch (const std::exception& e)
        {
            std::cout << key.toStdString() << ": " << e.what() << std::endl;
        }
    }
    // sort perks alphabetically
    std::sort(
        skillInfos.begin() + 1, skillInfos.end(),
              [this](const SkillInfo& lhs, const SkillInfo& rhs) {
                  return dict.getTranslationFor(lhs.getName())
                             .compare(dict.getTranslationFor(rhs.getName())) < 0;
              });
    for (size_t i = 1; i < skillInfos.size(); ++i)
    {
        mapSkillNameToId_[skillInfos[i].getName()] = i;
    }

    auto jsonArmours = json[JSON_ARMOURS].toObject();
    for (const auto& key : jsonArmours.keys())
    {
        try
        {
            if (!jsonArmours[key].isObject())
            {
                std::cout << "found non object armour entry: " << key.toStdString() << std::endl;
                continue;
            }
            auto armour = jsonArmours[key].toObject();
            if (!util::json::parameterCheck(armour, armourParameters))
            {
                std::cout << "found non conforming armour entry: " << key.toStdString()
                          << std::endl;
                continue;
            }
            auto type = getArmourType(armour[JSON_TYPE].toString().toStdString());
            std::string name = armour[JSON_NAME].toString().toStdString();
            std::string description = armour[JSON_DESCRIPTION].toString().toStdString();
            Elements elementalResistance;
            if (armour.contains(JSON_STRENGTH) && armour[JSON_STRENGTH].isString())
                setElement(elementalResistance, armour[JSON_STRENGTH].toString().toStdString(), 25);
            if (armour.contains(JSON_WEAKNESS) && armour[JSON_WEAKNESS].isString())
                setElement(elementalResistance, armour[JSON_WEAKNESS].toString().toStdString(),
                           -25);
            SkillType cell(None);
            if (armour.contains(JSON_CELLS) && armour[JSON_CELLS].isString())
                cell = getSkillType(armour[JSON_CELLS].toString().toStdString());

            auto resistances = armour[JSON_RESISTANCE].toObject();

            Skill normalSkills, maelstromSkills, heroicSkills;
            std::vector<std::string> uniqueSkills, uniqueSkillsMaelstrom, uniqueSkillsHeroic;

            if (armour.contains(JSON_PERKS))
            {
                normalSkills =
                    util::json::getSkillFromTo(armour[JSON_PERKS], 0, 5, mapSkillNameToId_);
                maelstromSkills =
                    util::json::getSkillFromTo(armour[JSON_PERKS], 6, 9, mapSkillNameToId_);
                heroicSkills =
                    util::json::getSkillFromTo(armour[JSON_PERKS], 10, 15, mapSkillNameToId_);
            }
            if (armour.contains(JSON_UNIQUE_EFFECT))
            {
                uniqueSkills =
                    util::json::getUniqueSkillsFromJson(armour[JSON_UNIQUE_EFFECT], dict, 5);
                uniqueSkillsMaelstrom =
                    util::json::getUniqueSkillsFromJson(armour[JSON_UNIQUE_EFFECT], dict, 9);
                uniqueSkillsHeroic =
                    util::json::getUniqueSkillsFromJson(armour[JSON_UNIQUE_EFFECT], dict, 15);
            }

            armours[type].emplace_back(type, name, description, 5, elementalResistance,
                                       uniqueSkills, cell, normalSkills);
            armours[type].emplace_back(type, name, description, 9, elementalResistance,
                                       uniqueSkillsMaelstrom, cell, maelstromSkills);
            armours[type].emplace_back(type, name, description, 15, elementalResistance,
                                       uniqueSkillsHeroic, cell, heroicSkills);

            dict.addEntry(name, name);
            dict.addEntry(description, description);
        }
        catch (const std::exception& e)
        {
            std::cout << key.toStdString() << ": " << e.what() << std::endl;
        }
    }
    auto jsonWeapons = json[JSON_WEAPONS].toObject();
    for (const auto& key : jsonWeapons.keys())
    {
        try
        {
            if (!jsonWeapons[key].isObject())
            {
                std::cout << "found non object weapon entry: " << key.toStdString() << std::endl;
                continue;
            }
            auto weapon = jsonWeapons[key].toObject();
            if (!util::json::parameterCheck(weapon, weaponParameters))
            {
                std::cout << "found non conforming weapon entry: " << key.toStdString()
                          << std::endl;
                continue;
            }

            auto type = getWeaponType(weapon[JSON_TYPE].toString().toStdString());
            std::string name = weapon[JSON_NAME].toString().toStdString();
            std::string description = weapon[JSON_DESCRIPTION].toString().toStdString();
            Elements elementalDamage;
            if (weapon.contains(JSON_ELEMENTAL) && weapon[JSON_ELEMENTAL].isString())
                setElement(elementalDamage, weapon[JSON_ELEMENTAL].toString().toStdString(), 80);
            SkillType cell1(None), cell2(None);
            if (weapon.contains(JSON_CELLS) && weapon[JSON_CELLS].isArray())
            {
                auto array = weapon[JSON_CELLS].toArray();
                auto it = array.begin();
                if (it == array.end())
                {
                    std::cout << key.toStdString() << ": it1 no iterator" << std::endl;
                    continue;
                }
                if (it->isString())
                    cell1 = getSkillType(it->toString().toStdString());
                else
                {
                    std::cout << key.toStdString() << ": non string cell" << std::endl;
                    continue;
                }
                ++it;
                if (it == array.end())
                {
                    std::cout << key.toStdString() << ": it2 no iterator" << std::endl;
                    continue;
                }
                if (it->isString())
                    cell2 = getSkillType(it->toString().toStdString());
                else
                {
                    std::cout << key.toStdString() << ": non string cell" << std::endl;
                    continue;
                }
            }

            Skill normalSkills, maelstromSkills, heroicSkills;
            std::vector<std::string> uniqueSkills, uniqueSkillsMaelstrom, uniqueSkillsHeroic;

            if (weapon.contains(JSON_PERKS))
            {
                normalSkills =
                    util::json::getSkillFromTo(weapon[JSON_PERKS], 0, 5, mapSkillNameToId_);
                maelstromSkills =
                    util::json::getSkillFromTo(weapon[JSON_PERKS], 6, 9, mapSkillNameToId_);
                heroicSkills =
                    util::json::getSkillFromTo(weapon[JSON_PERKS], 10, 15, mapSkillNameToId_);
            }
            if (weapon.contains(JSON_UNIQUE_EFFECT))
            {
                uniqueSkills =
                    util::json::getUniqueSkillsFromJson(weapon[JSON_UNIQUE_EFFECT], dict, 5);
                uniqueSkillsMaelstrom =
                    util::json::getUniqueSkillsFromJson(weapon[JSON_UNIQUE_EFFECT], dict, 9);
                uniqueSkillsHeroic =
                    util::json::getUniqueSkillsFromJson(weapon[JSON_UNIQUE_EFFECT], dict, 15);
            }

            weapons[type].emplace_back(type, name, description, 5, elementalDamage, uniqueSkills,
                                       cell1, cell2, normalSkills);
            weapons[type].emplace_back(type, name, description, 9, elementalDamage,
                                       uniqueSkillsMaelstrom, cell1, cell2, maelstromSkills);
            weapons[type].emplace_back(type, name, description, 15, elementalDamage,
                                       uniqueSkillsHeroic, cell1, cell2, heroicSkills);

            dict.addEntry(name, name);
            dict.addEntry(description, description);
        }
        catch (const std::exception& e)
        {
            std::cout << key.toStdString() << ": " << e.what() << std::endl;
        }
    }
}

void Gear::Armoury::setElement(Elements& element, const std::string& name, int value)
{
    if (name == "Umbral")
        element.Umbral = value;
    else if (name == "Radiant")
        element.Radiant = value;
    else if (name == "Shock")
        element.Shock = value;
    else if (name == "Frost")
        element.Ice = value;
    else if (name == "Blaze")
        element.Fire = value;
    else if (name == "Terra")
        element.Terra = value;
    else
        throw std::logic_error("Unknown element " + name);
}

Gear::SkillType Gear::Armoury::getSkillType(const std::string& type) const
{
    if (type == "Power")
        return SkillType::Power;
    if (type == "Technique")
        return SkillType::Technique;
    if (type == "Mobility")
        return SkillType::Mobility;
    if (type == "Defence")
        return SkillType::Defense;
    if (type == "Utility")
        return SkillType::Utility;
    throw std::logic_error("Unknown skill type " + type);
}

Gear::ArmourType Gear::Armoury::getArmourType(const std::string& type) const
{
    if (type == "Arms")
        return ArmourType::Arms;
    if (type == "Legs")
        return ArmourType::Legs;
    if (type == "Torso")
        return ArmourType::Torso;
    if (type == "Head")
        return ArmourType::Head;
    throw std::logic_error("Unknown armour type " + type);
}

Gear::WeaponType Gear::Armoury::getWeaponType(const std::string& type) const
{
    if (type == "Axe")
        return WeaponType::Axe;
    if (type == "Chain Blades")
        return WeaponType::Chainblades;
    if (type == "Hammer")
        return WeaponType::Hammer;
    if (type == "Sword")
        return WeaponType::Sword;
    if (type == "War Pike")
        return WeaponType::Pike;
    if (type == "Repeater")
        return WeaponType::Reapeater;
    throw std::logic_error("Unknown weapon type " + type);
}

bool Gear::Armoury::filterGear(const Gear& gear, const Options& options) const
{
    int level = 0;
    if (options.tier == 1)
        level = 5;
    else if (options.tier == 2)
        level = 9;
    else if (options.tier == 3)
        level = 15;
    else
    {
        std::cout << "WARNING: unknown tier:" << options.tier << std::endl;
        return true;
    }
    if (options.useLowerTierArmour)
    {
        return gear.getLevel() > level;
    }
    else
    {
        return gear.getLevel() != level;
    }
}

bool Gear::Armoury::filterWeapon(const Weapon& weapon, const Options& options) const
{
    if (options.weaponElement != Element::All && weapon.getType() != WeaponType::Reapeater)
    {
        const auto& elements_ = weapon.getElements();
        switch (options.weaponElement)
        {
        case Element::Elementless:
            if (elements_.Fire > 0 || elements_.Ice > 0 || elements_.Shock > 0 ||
                elements_.Terra > 0 || elements_.Radiant > 0 || elements_.Umbral > 0)
                return true;
            break;
        case Element::Fire:
            if (elements_.Fire == 0)
                return true;
            break;
        case Element::Ice:
            if (elements_.Ice == 0)
                return true;
            break;
        case Element::Shock:
            if (elements_.Shock == 0)
                return true;
            break;
        case Element::Terra:
            if (elements_.Terra == 0)
                return true;
            break;
        case Element::Radiant:
            if (elements_.Radiant == 0)
                return true;
            break;
        case Element::Umbral:
            if (elements_.Umbral == 0)
                return true;
            break;
        }
    }
    return filterGear(weapon, options);
}

void Gear::Armoury::loadData() { load(); }