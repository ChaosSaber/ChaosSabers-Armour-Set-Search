#include "gear/ArmourSet.hpp"
#include "hashids.h"
#include <algorithm>
#include <gear/Armoury.hpp>
#include <iostream>
#include <sstream>

Gear::ArmourSet::ArmourSet(const Armoury& armoury, const Armour& head, const Armour& torso,
                           const Armour& arms, const Armour& legs, const Weapon& weapon,
                           const Cell& lantern)
    : armoury_(armoury), head_(head), torso_(torso), legs_(legs), arms_(arms), weapon_(weapon),
      lantern_(lantern), innateSkills_(armoury), cellSlotsPerType_({})
{
    init();
}

Gear::ArmourSet::ArmourSet(const Armoury& armoury, Armour&& head, Armour&& torso, Armour&& arms,
                           Armour&& legs, Weapon&& weapon, Cell&& lantern)
    : armoury_(armoury), head_(std::move(head)), torso_(std::move(torso)), legs_(std::move(legs)),
      arms_(std::move(arms)), weapon_(std::move(weapon)), lantern_(std::move(lantern)),
      innateSkills_(armoury), cellSlotsPerType_({})
{
    init();
}

void Gear::ArmourSet::init()
{
    gear_.push_back(&weapon_);
    gear_.push_back(&head_);
    gear_.push_back(&torso_);
    gear_.push_back(&arms_);
    gear_.push_back(&legs_);
    for (const auto& gear : gear_)
    {
        innateSkills_ += gear->getInnateSkills();
        for (const auto& cell : gear->getCellList())
            cellSlotsPerType_[cell.first.getCellType()] += cell.second;
    }
    ++cellSlotsPerType_[lantern_.getCellType()];
}

void Gear::ArmourSet::switchGear(const Gear& oldGear, const Gear& newGear)
{
    innateSkills_ += newGear.getInnateSkills();
    innateSkills_ -= oldGear.getInnateSkills();
    for (const auto& cell : oldGear.getCellList())
        --cellSlotsPerType_[cell.first.getCellType()];
    for (const auto& cell : newGear.getCellList())
        ++cellSlotsPerType_[cell.first.getCellType()];
}

Gear::ArmourSet::ArmourSet(const ArmourSet& other)
    : ArmourSet(other.armoury_, other.head_, other.torso_, other.arms_, other.legs_, other.weapon_,
                other.lantern_)
{
}

Gear::ArmourSet::ArmourSet(ArmourSet&& other)
    : ArmourSet(other.armoury_, std::move(other.head_), std::move(other.torso_),
                std::move(other.arms_), std::move(other.legs_), std::move(other.weapon_),
                std::move(other.lantern_))
{
}

bool Gear::ArmourSet::hasFreeCellSlotFor(SkillType type) const
{
    if (lantern_.isEmpty() && lantern_.getCellType() == type)
        return true;
    for (const auto part : gear_)
    {
        if (part->hasFreeCellSlotFor(type))
            return true;
    }
    return false;
}

bool Gear::ArmourSet::addCell(Cell cell)
{
    if (lantern_.isEmpty() && lantern_.getCellType() == cell.getCellType())
    {
        lantern_ = std::move(cell);
        return true;
    }
    for (auto part : gear_)
    {
        if (part->addCell(cell))
            return true;
    }
    return false;
}

Gear::SkillList Gear::ArmourSet::getSkills() const
{
    SkillList skills;
    for (const auto part : gear_)
        skills += part->getSkills();
    skills += lantern_.getSkill();
    return skills;
}

size_t Gear::ArmourSet::getSkillPointsFor(size_t skillId) const
{
    return innateSkills_.getWantedLevel(skillId);
}

Gear::CellList Gear::ArmourSet::getCellList() const
{
    CellList cells = lantern_;
    for (const auto part : gear_)
    {
        cells += part->getCellList();
    }
    return cells;
}

size_t Gear::ArmourSet::getCellSlotCountFor(SkillType type) const
{
    return cellSlotsPerType_[type];
}

void Gear::ArmourSet::removeAllCells()
{
    for (auto& gear : gear_)
    {
        gear->removeAllCells();
    }
    lantern_.clear();
}

const Gear::Armour& Gear::ArmourSet::getHead() const { return head_; }

const Gear::Armour& Gear::ArmourSet::getTorso() const { return torso_; }

const Gear::Armour& Gear::ArmourSet::getArms() const { return arms_; }

const Gear::Armour& Gear::ArmourSet::getLegs() const { return legs_; }

const Gear::Weapon& Gear::ArmourSet::getWeapon() const { return weapon_; }

const Gear::Cell& Gear::ArmourSet::getLantern() const { return lantern_; }

Gear::SkillList Gear::ArmourSet::getAdditionalSkills(const SkillList& wantedSkills) const
{
    SkillList skills;
    for (const auto& skill : getSkills())
    {
        if (!wantedSkills.contains(skill))
            skills += skill;
    }
    return skills;
}

std::string Gear::ArmourSet::exportToText(const Dictionary& dict, const Armoury& armoury) const
{
    std::stringstream text;

    size_t maxGearLength = 0;
    for (const auto& gear : this->gear_)
    {
        auto length = gear->getGearInfo(dict).length();
        if (length > maxGearLength)
            maxGearLength = length;
    }

    auto cells = getCellList();
    cells.sort();
    constexpr size_t cellCountSize = 3; // "1x ", "2x ",...
    size_t maxCellLength = 0;
    for (const auto& cell : cells)
    {
        auto length = cell.first.getCellInfo(dict, armoury).length();
        if (length > maxCellLength)
            maxCellLength = length;
    }
    maxCellLength += cellCountSize;

    auto skills = getSkills();
    skills.sort();
    size_t maxSkillLength = 0;
    for (const auto& skill : skills)
    {
        auto length = skill.toString(dict, armoury).length();
        if (length > maxSkillLength)
            maxSkillLength = length;
    }

    size_t maxColoumnLength = std::max({maxGearLength, /*maxCellLength,*/ maxSkillLength}) + 1;
    size_t lineWidth = maxColoumnLength + std::max(maxColoumnLength, maxCellLength) + 2; //"| "
    size_t maxIt = std::max(gear_.size(), cells.size());
    for (size_t i = 0; i < maxIt; ++i)
    {
        size_t length = 0;
        if (i < gear_.size())
        {
            auto gearStr = gear_[i]->getGearInfo(dict);
            text << gearStr;
            length = gearStr.length();
        }
        text << std::string(maxColoumnLength - length, ' ') << "|";
        if (i < cells.size())
        {
            const auto& [cell, count] = cells[i];
            text << " " << count << "x " << cell.getCellInfo(dict, armoury);
        }
        text << std::endl;
    }
    text << std::string(lineWidth, '-') << std::endl;
    for (size_t i = 0; i < skills.size(); ++i)
    {
        if (i % 2 == 0)
        {
            auto skillStr = skills[i].toString(dict, armoury);
            text << skillStr;
            if (i + 1 != skills.size())
            {
                text << std::string(maxColoumnLength - skillStr.length() + 2,
                                    ' '); // we won't be using a '|' to separate the two coloumns
            }
        }
        else
        {
            text << skills[i].toString(dict, armoury);
            if (i < skills.size() - 1)
                text << std::endl;
        }
    }
    auto uniqueSkills = getUniqueSkills();
    if (!uniqueSkills.empty())
    {
        text << std::endl << std::string(lineWidth, '-') << std::endl;
        for (const auto& skill : uniqueSkills)
            text << skill << std::endl;
    }
    return text.str();
}

std::string Gear::ArmourSet::getHashIds(const Armoury& armoury) const
{
    std::vector<int> ids;
    ids.push_back(3); // version
    auto addGear = [&ids, &armoury](const Gear& gear) {
        ids.push_back(gear.getId());
        ids.push_back(gear.getLevel());
        for (const auto& cell : gear.getCellList())
        {
            for (size_t i = 0; i < cell.second; ++i)
            {
                ids.push_back(armoury.getCellId(cell.first));
            }
        }
    };
    addGear(weapon_);
    ids.push_back(0); // weapon part 1
    ids.push_back(0); // weapon part 2
    ids.push_back(0); // weapon part 3
    ids.push_back(0); // weapon part 4
    ids.push_back(0); // weapon part 5
    ids.push_back(0); // weapon part 6
    addGear(head_);
    addGear(torso_);
    addGear(arms_);
    addGear(legs_);
    ids.push_back(1); // lantern
    ids.push_back(armoury.getCellId(lantern_));
    hashidsxx::Hashids hash("spicy");
    return hash.encode(ids.begin(), ids.end());
}

bool Gear::ArmourSet::hasUniqueSkill() const
{
    for (auto gear : this->gear_)
        if (gear->hasUniqueSkill())
            return true;
    return false;
}

std::vector<std::string> Gear::ArmourSet::getUniqueSkills() const
{
    std::vector<std::string> uniqueSkills;
    for (auto gear : this->gear_)
    {
        if (!gear->hasUniqueSkill())
            continue;
        auto unique = gear->getUniqueSkills();
        uniqueSkills.insert(uniqueSkills.end(), unique.begin(), unique.end());
    }
    return uniqueSkills;
}

void Gear::ArmourSet::setHead(const Armour& head)
{
    switchGear(head_, head);
    head_ = head;
}

void Gear::ArmourSet::setTorso(const Armour& torso)
{
    switchGear(torso_, torso);
    torso_ = torso;
}

void Gear::ArmourSet::setArms(const Armour& arms)
{
    switchGear(arms_, arms);
    arms_ = arms;
}

void Gear::ArmourSet::setLegs(const Armour& legs)
{
    switchGear(legs_, legs);
    legs_ = legs;
}

void Gear::ArmourSet::setWeapon(const Weapon& weapon)
{
    switchGear(weapon_, weapon);
    weapon_ = weapon;
}
