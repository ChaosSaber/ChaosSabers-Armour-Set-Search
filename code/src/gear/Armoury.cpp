#include "gear/Armoury.hpp"
#include "util/string.hpp"
#include <fstream>
#include <iostream>

Gear::Armoury::Armoury() : notFound("", None)
{
    weapons[Sword] = loadWeapons("data/Swords.txt", Sword);
    weapons[Hammer] = loadWeapons("data/Hammer.txt", Hammer);
    weapons[Chainblades] = loadWeapons("data/Chainblades.txt", Chainblades);
    weapons[Axe] = loadWeapons("data/Axe.txt", Axe);
    weapons[Pike] = loadWeapons("data/Pike.txt", Pike);
    armours[Head] = loadArmour("data/Heads.txt", Head);
    armours[Torso] = loadArmour("data/Torsos.txt", Torso);
    armours[Arms] = loadArmour("data/Arms.txt", Arms);
    armours[Legs] = loadArmour("data/Legs.txt", Legs);
    skillInfos = loadSkillInfos("data/Skills.txt");
}

const Gear::SkillInfo &Gear::Armoury::getSkillInfoFor(const std::string &name) const
{
    for (const auto &skillInfo : skillInfos)
    {
        if (skillInfo.getName() == name)
            return skillInfo;
    }
    // TODO: errorhandling if a skill is not found
    return notFound;
}

std::vector<Gear::Weapon> Gear::Armoury::getWeaponsWithSkill(const std::vector<Skill> &skills,
                                                             WeaponType type) const
{
    std::vector<Weapon> weapons;
    if (this->weapons.count(type) == 0)
        return weapons;
    for (const auto &weapon : this->weapons.at(type))
    {
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
                                                            ArmourType type) const
{
    std::vector<Armour> armours;
    if (this->armours.count(type) == 0)
        return armours;
    for (const auto &armour : this->armours.at(type))
    {
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

std::vector<Gear::SkillInfo> Gear::Armoury::loadSkillInfos(const std::string &fileName)
{
    std::vector<SkillInfo> skills;
    std::ifstream file(fileName);
    if (!file.is_open())
    {
        // TODO: errorhandling
        std::cout << "couldn't open file " << fileName << std::endl;
        return skills;
    }
    std::string line;
    while (std::getline(file, line))
    {
        if (util::string::isComment(line))
            continue;
        auto tmp = util::string::split(line, ';');
        if (tmp.size() >= 2)
        {
            try
            {
                auto type = std::stoi(tmp[1]);
                skills.push_back(SkillInfo(util::string::toLowerCopy(tmp[0]), (SkillType)type));
            }
            catch (std::invalid_argument)
            {
                // TODO: errorhandling
                std::cout << "Couldn't convert line: " << line;
            }
            catch (std::out_of_range)
            {
                // TODO: errorhandling
                std::cout << "Couldn't convert line: " << line;
            }
        }
        else
        {
            // errorhandling or just ignore?
            std::cout << "Couldn't convert line: " << line;
        }
    }
    return skills;
}

std::vector<Gear::Armour> Gear::Armoury::loadArmour(const std::string &fileName, ArmourType type)
{
    std::vector<Armour> armours;
    std::ifstream file(fileName);
    if (!file.is_open())
    {
        // TODO: errorhandling
        std::cout << "couldn't open file " << fileName << std::endl;
        return armours;
    }
    std::string line;
    while (std::getline(file, line))
    {
        if (util::string::isComment(line))
            continue;
        auto tmp = util::string::split(line, ';');
        if (tmp.size() < 8)
        {
            std::cout << "Couldn't convert line: " << line;
            continue;
        }
        try
        {
            auto name = util::string::toLowerCopy(tmp[0]);
            auto defense = std::stoi(tmp[1]);
            auto elems = util::string::split(tmp[2], '|');
            if (elems.size() != 5)
                continue;
            Elements elementalResistance;
            elementalResistance.Fire = std::stoi(elems[0]);
            elementalResistance.Ice = std::stoi(elems[1]);
            elementalResistance.Shock = std::stoi(elems[2]);
            elementalResistance.Radiant = std::stoi(elems[3]);
            elementalResistance.Umbral = std::stoi(elems[4]);
            Skill skill1(util::string::toLowerCopy(tmp[3]), std::stoi(tmp[4]));
            Skill skill2(util::string::toLowerCopy(tmp[5]), std::stoi(tmp[6]));
            SkillType cellType = (SkillType)std::stoi(tmp[7]);
            armours.push_back(
                Armour(type, name, defense, elementalResistance, skill1, skill2, cellType));
        }
        catch (std::invalid_argument)
        {
            // TODO: errorhandling
            std::cout << "Couldn't convert line: " << line;
        }
        catch (std::out_of_range)
        {
            // TODO: errorhandling
            std::cout << "Couldn't convert line: " << line;
        }
    }
    return armours;
}

std::vector<Gear::Weapon> Gear::Armoury::loadWeapons(const std::string &fileName, WeaponType type)
{
    std::vector<Weapon> weapons;
    std::ifstream file(fileName);
    if (!file.is_open())
    {
        // TODO: errorhandling
        std::cout << "couldn't open file " << fileName << std::endl;
        return weapons;
    }
    std::string line;
    while (std::getline(file, line))
    {
        if (util::string::isComment(line))
            continue;
        auto tmp = util::string::split(line, ';');
        if (tmp.size() < 9)
        {
            // TODO: errorhandling
            std::cout << "Couldn't convert line: " << line;
            continue;
        }
        try
        {
            auto name = util::string::toLowerCopy(tmp[0]);
            auto damage = std::stoi(tmp[1]);
            auto elems = util::string::split(tmp[2], '|');
            if (elems.size() != 5)
                continue;
            Elements elementalDamage;
            elementalDamage.Fire = std::stoi(elems[0]);
            elementalDamage.Ice = std::stoi(elems[1]);
            elementalDamage.Shock = std::stoi(elems[2]);
            elementalDamage.Radiant = std::stoi(elems[3]);
            elementalDamage.Umbral = std::stoi(elems[4]);
            Skill skill1(util::string::toLowerCopy(tmp[3]), std::stoi(tmp[4]));
            Skill skill2(util::string::toLowerCopy(tmp[5]), std::stoi(tmp[6]));
            SkillType cellType1 = (SkillType)std::stoi(tmp[7]);
            SkillType cellType2 = (SkillType)std::stoi(tmp[8]);
            weapons.push_back(
                Weapon(type, name, damage, elementalDamage, skill1, skill2, cellType1, cellType2));
        }
        catch (std::invalid_argument)
        {
            // TODO: errorhandling
            std::cout << "Couldn't convert line: " << line;
        }
        catch (std::out_of_range)
        {
            // TODO: errorhandling
            std::cout << "Couldn't convert line: " << line;
        }
    }
    return weapons;
}

std::vector<const Gear::SkillInfo *> Gear::Armoury::getSkills(SkillType filter) const
{
    std::vector<const SkillInfo *> skills;
    for (const auto &skill : skillInfos)
        if (filter == None || filter == skill.getType())
            skills.push_back(&skill);
    return skills;
}

Gear::SkillType Gear::Armoury::getSkillTypeFor(const std::string &skillName) const
{
    return getSkillInfoFor(skillName).getType();
}

const Gear::Armour &Gear::Armoury::getArmour(std::string name) const
{
    util::string::toLower(name);
    for (const auto &armours : this->armours)
        for (const auto &armour : armours.second)
            if (armour.getName() == name)
                return armour;
    std::stringstream ss;
    ss << "There is no armour with the key " << name;
    throw std::exception(ss.str().c_str());
}

const Gear::Weapon &Gear::Armoury::getWeapon(std::string name) const
{
    util::string::toLower(name);
    for (const auto &weapons : this->weapons)
        for (const auto &weapon : weapons.second)
            if (weapon.getName() == name)
                return weapon;
    std::stringstream ss;
    ss << "There is no weapon with the key " << name;
    throw std::exception(ss.str().c_str());
}
