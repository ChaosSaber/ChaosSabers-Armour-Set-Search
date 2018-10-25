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

Gear::Armoury::Armoury(Dictionary &dict)
    : notFound("", "", None, std::vector<std::string>()), dict(dict)
{
}

const Gear::SkillInfo &Gear::Armoury::getSkillInfoFor(const std::string &name) const
{
    for (const auto &skillInfo : skillInfos)
    {
        if (skillInfo.getName() == name)
            return skillInfo;
    }
    std::cout << "Couldn't find skill " << name << std::endl;
    return notFound;
}

std::vector<Gear::Weapon> Gear::Armoury::getWeaponsWithSkill(const std::vector<Skill> &skills,
                                                             WeaponType type,
                                                             const Options &options) const
{
    std::vector<Weapon> weapons;
    if (this->weapons.count(type) == 0)
        return weapons;
    for (const auto &weapon : this->weapons.at(type))
    {
        if (filterGear(weapon, options))
            continue;
        for (const auto &skill : skills)
        {
            if (weapon.getSkillPointsFor(skill.getName()) > 0 ||
                weapon.hasFreeCellSlotFor(getSkillInfoFor(skill.getName()).getType()))
            {
                weapons.push_back(weapon);
                break;
            }
        }
    }
    return weapons;
}

std::vector<Gear::Armour> Gear::Armoury::getArmourWithSkill(const std::vector<Skill> &skills,
                                                            ArmourType type,
                                                            const Options &options) const
{
    std::vector<Armour> armours;
    if (this->armours.count(type) == 0)
        return armours;
    for (const auto &armour : this->armours.at(type))
    {
        if (filterGear(armour, options))
            continue;
        for (const auto &skill : skills)
        {
            if (armour.getSkillPointsFor(skill.getName()) > 0 ||
                armour.hasFreeCellSlotFor(getSkillInfoFor(skill.getName()).getType()))
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

std::vector<const Gear::SkillInfo *> Gear::Armoury::getSkills(SkillType filter) const
{
    std::vector<const SkillInfo *> skills;
    for (const auto &skill : skillInfos)
        if (filter == None && skill.getType() || filter == skill.getType())
            skills.push_back(&skill);
    return skills;
}

Gear::SkillType Gear::Armoury::getSkillTypeFor(const std::string &skillName) const
{
    return getSkillInfoFor(skillName).getType();
}

const Gear::Armour &Gear::Armoury::getArmour(std::string name, bool heroic) const
{
    for (const auto &armours : this->armours)
        for (const auto &armour : armours.second)
            if (armour.getName() == name && isCorrectGear(armour, heroic))
                return armour;
    std::stringstream ss;
    ss << "There is no armour with the key " << name;
    throw std::exception(ss.str().c_str());
}

const Gear::Weapon &Gear::Armoury::getWeapon(std::string name, bool heroic) const
{
    for (const auto &weapons : this->weapons)
        for (const auto &weapon : weapons.second)
            if (weapon.getName() == name && isCorrectGear(weapon, heroic))
                return weapon;
    std::stringstream ss;
    ss << "There is no weapon with the key " << name;
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
#define JSON_TIER "tier"
#define JSON_RESISTANCE "resistance"
#define JSON_PERKS "perks"
#define JSON_UNIQUE_EFFECT "unique_effects"
#define JSON_ELEMENTAL "elemental"
#define JSON_POWER "power"
#define JSON_EFFECTS "effects"
#define JSON_RARITY "rarity"

const std::vector<util::json::JsonParameter> armourParameters = {
    {JSON_NAME, QJsonValue::Type::String},       {JSON_DESCRIPTION, QJsonValue::Type::String},
    {JSON_TYPE, QJsonValue::Type::String},       {JSON_TIER, QJsonValue::Type::Double},
    {JSON_RESISTANCE, QJsonValue::Type::Object}, {JSON_RARITY, QJsonValue::Type::String}};

const std::vector<util::json::JsonParameter> weaponParameters = {
    {JSON_NAME, QJsonValue::Type::String},
    {JSON_DESCRIPTION, QJsonValue::Type::String},
    {JSON_TYPE, QJsonValue::Type::String},
    {JSON_TIER, {QJsonValue::Type::Double, QJsonValue::Type::Array}},
    {JSON_POWER, QJsonValue::Type::Object},
    {JSON_RARITY, QJsonValue::Type::String}};

const std::vector<util::json::JsonParameter> skillParameters = {
    {JSON_NAME, QJsonValue::Type::String},
    {JSON_DESCRIPTION, QJsonValue::Type::String},
    {JSON_TYPE, QJsonValue::Type::String},
    {JSON_EFFECTS, QJsonValue::Type::Object}};

void Gear::Armoury::load(const std::string &fileName)
{
    // TODO: implement fail safety, like loading backup data
    weapons.clear();
    armours.clear();
    skillInfos.clear();
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
    auto jsonArmours = json[JSON_ARMOURS].toObject();
    for (const auto &key : jsonArmours.keys())
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
            unsigned int tier = armour[JSON_TIER].toInt();
            auto rarity = getRarity(armour);
            Elements elementalResistance;
            if (armour.contains(JSON_STRENGTH) && armour[JSON_STRENGTH].isString())
                setElement(elementalResistance, armour[JSON_STRENGTH].toString().toStdString(), 10);
            if (armour.contains(JSON_WEAKNESS) && armour[JSON_WEAKNESS].isString())
                setElement(elementalResistance, armour[JSON_WEAKNESS].toString().toStdString(), -5);
            SkillType cell(None);
            if (armour.contains(JSON_CELLS) && armour[JSON_CELLS].isString())
                cell = getSkillType(armour[JSON_CELLS].toString().toStdString());

            auto resistances = armour[JSON_RESISTANCE].toObject();
            int minDef = util::json::getValueForLevel(resistances, "0");
            if (tier < 5) // non maelstroem stuff
            {
                int maxDef = util::json::getMaxValue(resistances);
                std::vector<Skill> skills;
                if (armour.contains(JSON_PERKS) && armour[JSON_PERKS].isArray())
                    for (const auto &jsonPerk : armour[JSON_PERKS].toArray())
                        skills.push_back(util::json::jsonToSkill(jsonPerk));
                std::vector<std::string> uniqueSkills;
                if (armour.contains(JSON_UNIQUE_EFFECT) && armour[JSON_UNIQUE_EFFECT].isArray())
                    for (const auto &jsonUnique : armour[JSON_UNIQUE_EFFECT].toArray())
                        uniqueSkills.push_back(util::json::jsonToUniqueSkill(jsonUnique, dict));
                armours[type].push_back(Armour(type, name, description, tier, minDef, maxDef,
                                               elementalResistance, skills, uniqueSkills, cell,
                                               rarity));
            }
            else // maelstroem stuff (heroic/normal)
            {
                int maxDef = util::json::getValueForLevel(resistances, "5");
                int maxDefHeroic = util::json::getValueForLevel(resistances, "10");
                std::vector<Skill> skills, skillsHeroic;
                if (armour.contains(JSON_PERKS) && armour[JSON_PERKS].isArray())
                    for (const auto &jsonPerk : armour[JSON_PERKS].toArray())
                        util::json::addMaelstromSkill(skills, skillsHeroic, jsonPerk,
                                                      util::json::jsonToSkill(jsonPerk));
                std::vector<std::string> uniqueSkills, uniqueSkillsHeroic;
                if (armour.contains(JSON_UNIQUE_EFFECT) && armour[JSON_UNIQUE_EFFECT].isArray())
                    for (const auto &jsonUnique : armour[JSON_UNIQUE_EFFECT].toArray())
                        util::json::addMaelstromSkill(
                            uniqueSkills, uniqueSkillsHeroic, jsonUnique,
                            util::json::jsonToUniqueSkill(jsonUnique, dict));
                if (rarity != Exotic)
                {
                    armours[type].push_back(Armour(type, name, description, 5, minDef, maxDef,
                                                   elementalResistance, skills, uniqueSkills, cell,
                                                   rarity));
                }
                armours[type].push_back(Armour(type, name, description, 6, minDef, maxDefHeroic,
                                               elementalResistance, skillsHeroic,
                                               uniqueSkillsHeroic, cell, rarity));
            }
            dict.addEntry(name, name);
            dict.addEntry(description, description);
        }
        catch (const std::exception &e)
        {
            std::cout << key.toStdString() << ": " << e.what() << std::endl;
        }
    }
    auto jsonWeapons = json[JSON_WEAPONS].toObject();
    for (const auto &key : jsonWeapons.keys())
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
            std::vector<int> tiers = getTiers(weapon);
            if (tiers.size() == 0)
                continue;
            auto rarity = getRarity(weapon);
            Elements elementalDamage;
            if (weapon.contains(JSON_ELEMENTAL) && weapon[JSON_ELEMENTAL].isString())
                setElement(elementalDamage, weapon[JSON_ELEMENTAL].toString().toStdString(), 40);
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
            auto damage = weapon[JSON_POWER].toObject();
            int minDamage = util::json::getValueForLevel(damage, "0");
            if (tiers.size() > 1 || tiers[0] < 5)
            {
                int maxDamage = util::json::getMaxValue(damage);
                std::vector<Skill> skills;
                if (weapon.contains(JSON_PERKS) && weapon[JSON_PERKS].isArray())
                    for (const auto &jsonPerk : weapon[JSON_PERKS].toArray())
                        skills.push_back(util::json::jsonToSkill(jsonPerk));
                std::vector<std::string> uniqueSkills;
                if (weapon.contains(JSON_UNIQUE_EFFECT) && weapon[JSON_UNIQUE_EFFECT].isArray())
                    for (const auto &jsonUnique : weapon[JSON_UNIQUE_EFFECT].toArray())
                        uniqueSkills.push_back(util::json::jsonToUniqueSkill(jsonUnique, dict));
                weapons[type].push_back(Weapon(type, name, description, tiers, minDamage, maxDamage,
                                               elementalDamage, skills, uniqueSkills, cell1, cell2,
                                               rarity));
            }
            else
            {
                int maxDamage = util::json::getValueForLevel(damage, "5");
                int maxDamageHeroic = util::json::getValueForLevel(damage, "10");
                std::vector<Skill> skills, skillsHeroic;
                if (weapon.contains(JSON_PERKS) && weapon[JSON_PERKS].isArray())
                    for (const auto &jsonPerk : weapon[JSON_PERKS].toArray())
                        util::json::addMaelstromSkill(skills, skillsHeroic, jsonPerk,
                                                      util::json::jsonToSkill(jsonPerk));
                std::vector<std::string> uniqueSkills, uniqueSkillsHeroic;
                if (weapon.contains(JSON_UNIQUE_EFFECT) && weapon[JSON_UNIQUE_EFFECT].isArray())
                    for (const auto &jsonUnique : weapon[JSON_UNIQUE_EFFECT].toArray())
                        util::json::addMaelstromSkill(
                            uniqueSkills, uniqueSkillsHeroic, jsonUnique,
                            util::json::jsonToUniqueSkill(jsonUnique, dict));
                if (rarity != Exotic)
                {
                    weapons[type].push_back(Weapon(type, name, description, 5, minDamage, maxDamage,
                                                   elementalDamage, skills, uniqueSkills, cell1,
                                                   cell2, rarity));
                }
                weapons[type].push_back(Weapon(type, name, description, 6, minDamage,
                                               maxDamageHeroic, elementalDamage, skillsHeroic,
                                               uniqueSkillsHeroic, cell1, cell2, rarity));
            }
            dict.addEntry(name, name);
            dict.addEntry(description, description);
        }
        catch (const std::exception &e)
        {
            std::cout << key.toStdString() << ": " << e.what() << std::endl;
        }
    }
    auto jsonPerks = json[JSON_PERKS].toObject();
    for (const auto &key : jsonPerks.keys())
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
        catch (const std::exception &e)
        {
            std::cout << key.toStdString() << ": " << e.what() << std::endl;
        }
    }
}

void Gear::Armoury::setElement(Elements &element, const std::string &name, int value)
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
    else
        throw std::logic_error("Unknown element " + name);
}

Gear::SkillType Gear::Armoury::getSkillType(const std::string &type) const
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

Gear::ArmourType Gear::Armoury::getArmourType(const std::string &type) const
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

Gear::WeaponType Gear::Armoury::getWeaponType(const std::string &type) const
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

Gear::Rarity Gear::Armoury::getRarity(const QJsonObject &gear) const
{
    std::string rarity = gear[JSON_RARITY].toString().toStdString();
    if (rarity == "common")
        return Rarity::Common;
    if (rarity == "uncommon")
        return Rarity::Uncommon;
    if (rarity == "rare")
        return Rarity::Rare;
    if (rarity == "epic")
        return Rarity::Epic;
    if (rarity == "exotic")
        return Rarity::Exotic;
    throw std::logic_error("Unknown rarity " + rarity);
}

std::vector<int> Gear::Armoury::getTiers(const QJsonObject &gear) const
{
    if (gear[JSON_TIER].type() == QJsonValue::Type::Double)
        return {gear[JSON_TIER].toInt()};
    else if (gear[JSON_TIER].type() == QJsonValue::Type::Array)
    {
        std::vector<int> tiers;
        for (const auto &tier : gear[JSON_TIER].toArray())
        {
            if (tier.isDouble())
                tiers.push_back(tier.toInt());
            else
            {
                std::cout << "WARNING: non int tier!" << std::endl;
            }
        }
        if (std::find(tiers.begin(), tiers.end(), 5) != tiers.end())
            tiers.push_back(6); // if it has a maelstroem variant it probably also has a heroic variant (needed for filtering)

        return tiers;
    }
    std::cout << "WARNING: unknown Tier type: " << gear[JSON_TIER].type() << std::endl;
    return std::vector<int>();
}

bool Gear::Armoury::isCorrectGear(const Gear &gear, bool heroic) const
{
    if (!heroic)
        return true;
    if (gear.getTiers().size() > 1)
    {
        // modular repeater
        // there is no distinction between heroic and not heroic here
        return true;
    }
    else if (gear.getTiers().size() == 1)
    {
        return gear.getTiers()[0] == 6;
    }
    else
    {
        std::cout << "WARNING: Gear without tier!" << std::endl;
        return false;
    }
}

bool Gear::Armoury::filterGear(const Gear &gear, const Options &options) const
{
    if (gear.getTiers().size() == 1)
    {
        // we have a regular weapon which is only on 1 tier
        auto tier = gear.getTiers()[0];
        if (tier > options.tier)
            return true;
        if (!options.useLowerTierArmour && isLowerTier(tier, options))
            return true;
        return false;
    }
    else if (gear.getTiers().size() > 1)
    {
        // we have a modular repeater
        bool available = false;
        for (auto tier : gear.getTiers())
        {
            if (tier <= options.tier)
            {
                available = true;
                break;
            }
        }
        if (!available)
            return true;
        bool onCurrentTier = false;
        for (auto tier : gear.getTiers())
        {
            if (!isLowerTier(tier, options))
            {
                onCurrentTier = true;
                break;
            }
        }
        if (!options.useLowerTierArmour && !onCurrentTier)
            return true;
        return false;
    }
    else
    {
        // we have something really strange here
        std::cout << "WARNING: Gear without tier!" << std::endl;
        return true;
    }
}

bool Gear::Armoury::isLowerTier(int gearTier, const Options &options) const
{
    switch (options.tier)
    {
    case 6: return gearTier < 6;
    case 5: return gearTier < 4; // tier 4 is pretty equal to tier 5
    default: return false;
    }
}

void Gear::Armoury::loadData() { load(); }