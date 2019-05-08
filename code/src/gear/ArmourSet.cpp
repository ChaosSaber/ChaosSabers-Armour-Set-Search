#include "gear/ArmourSet.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>

Gear::ArmourSet::ArmourSet(const Armour& head, const Armour& torso, const Armour& arms,
                           const Armour& legs, const Weapon& weapon, const Cell& lantern)
    : head(head), torso(torso), legs(legs), arms(arms), weapon(weapon), lantern(lantern)
{
    init();
}

Gear::ArmourSet::ArmourSet(Armour&& head, Armour&& torso, Armour&& arms, Armour&& legs,
                           Weapon&& weapon, Cell&& lantern)
    : head(std::move(head)), torso(std::move(torso)), legs(std::move(legs)), arms(std::move(arms)),
      weapon(std::move(weapon)), lantern(std::move(lantern))
{
    init();
}

void Gear::ArmourSet::init()
{
    gear.push_back(&weapon);
    gear.push_back(&head);
    gear.push_back(&torso);
    gear.push_back(&arms);
    gear.push_back(&legs);
}

Gear::ArmourSet::ArmourSet(const ArmourSet& other)
    : ArmourSet(other.head, other.torso, other.arms, other.legs, other.weapon, other.lantern)
{
}

// void Gear::swap(ArmourSet& first, ArmourSet& second)
//{
//    using std::swap;
//    swap(first.head, second.head);
//    swap(first.torso, second.torso);
//    swap(first.arms, second.arms);
//    swap(first.legs, second.legs);
//    swap(first.weapon, second.weapon);
//    swap(first.lantern, second.lantern);
//}

// Gear::ArmourSet& Gear::ArmourSet::operator=(ArmourSet other)
//{
//    swap(*this, other);
//    return *this;
//}

Gear::ArmourSet::ArmourSet(ArmourSet&& other)
    : ArmourSet(std::move(other.head), std::move(other.torso), std::move(other.arms),
                std::move(other.legs), std::move(other.weapon), std::move(other.lantern))
{
}

bool Gear::ArmourSet::hasFreeCellSlotFor(SkillType type) const
{
    if (lantern.isEmpty() && lantern.getCellType() == type)
        return true;
    for (const auto part : gear)
    {
        if (part->hasFreeCellSlotFor(type))
            return true;
    }
    return false;
}

bool Gear::ArmourSet::addCell(Cell cell)
{
    if (cell.getSkillName() == "medic" && lantern.isEmpty())
    {
        std::string ss;
        ss = "5";
    }
    if (lantern.isEmpty() && lantern.getCellType() == cell.getCellType())
    {
        lantern = std::move(cell);
        return true;
    }
    for (auto part : gear)
    {
        if (part->addCell(cell))
            return true;
    }
    return false;
}

std::string Gear::ArmourSet::getGearInfo(const Dictionary& dict) const
{
    std::stringstream ss;
    ss << weapon.getGearInfo(dict) << std::endl
       << head.getGearInfo(dict) << std::endl
       << torso.getGearInfo(dict) << std::endl
       << arms.getGearInfo(dict) << std::endl
       << legs.getGearInfo(dict) << std::endl
       << getCellList().toString(dict) << "Skills:" << std::endl
       << getSkills().toString(dict);
    return ss.str();
}

Gear::SkillList Gear::ArmourSet::getSkills() const
{
    SkillList skills;
    for (const auto part : gear)
        skills += part->getSkills();
    skills += lantern.getSkill();
    return skills;
}

int Gear::ArmourSet::getSkillPointsFor(const std::string& skill) const
{
    int sum = 0;
    sum += lantern.getSkillPointsFor(skill);
    for (const auto part : gear)
        sum += part->getSkillPointsFor(skill);
    return sum;
}

Gear::CellList Gear::ArmourSet::getCellList() const
{
    CellList cells = lantern;
    for (const auto part : gear)
    {
        cells += part->getCellList();
    }
    return cells;
}

const Gear::Armour& Gear::ArmourSet::getHead() const { return head; }

const Gear::Armour& Gear::ArmourSet::getTorso() const { return torso; }

const Gear::Armour& Gear::ArmourSet::getArms() const { return arms; }

const Gear::Armour& Gear::ArmourSet::getLegs() const { return legs; }

const Gear::Weapon& Gear::ArmourSet::getWeapon() const { return weapon; }

const Gear::Cell& Gear::ArmourSet::getLantern() const { return lantern; }

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

std::string Gear::ArmourSet::exportToText(const Dictionary& dict) const
{
    std::stringstream text;

    size_t maxGearLength = 0;
    for (const auto& gear : this->gear)
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
        auto length = cell.first.getCellInfo(dict).length();
        if (length > maxCellLength)
            maxCellLength = length;
    }
    maxCellLength += cellCountSize;

    auto skills = getSkills();
    skills.sort();
    size_t maxSkillLength = 0;
    for (const auto& skill : skills)
    {
        auto length = skill.toString(dict).length();
        if (length > maxSkillLength)
            maxSkillLength = length;
    }

    size_t maxColoumnLength = std::max({maxGearLength, /*maxCellLength,*/ maxSkillLength}) + 1;
    size_t lineWidth = maxColoumnLength + std::max(maxColoumnLength, maxCellLength) + 2; //"| "
    size_t maxIt = std::max(gear.size(), cells.size());
    for (size_t i = 0; i < maxIt; ++i)
    {
        size_t length = 0;
        if (i < gear.size())
        {
            auto gearStr = gear[i]->getGearInfo(dict);
            text << gearStr;
            length = gearStr.length();
        }
        text << std::string(maxColoumnLength - length, ' ') << "|";
        if (i < cells.size())
        {
            const auto& [cell, count] = cells[i];
            text << " " << count << "x " << cell.getCellInfo(dict);
        }
        text << std::endl;
    }
    text << std::string(lineWidth, '-') << std::endl;
    for (size_t i = 0; i < skills.size(); ++i)
    {
        if (i % 2 == 0)
        {
            auto skillStr = skills[i].toString(dict);
            text << skillStr;
            if (i + 1 != skills.size())
            {
                text << std::string(maxColoumnLength - skillStr.length() + 2,
                                    ' '); // we won't be using a '|' to separate the two coloumns
            }
        }
        else
        {
            text << skills[i].toString(dict);
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

std::string Gear::ArmourSet::exportToText2(const Dictionary& dict) const
{
    constexpr auto indentation = "    ";
    std::vector<std::string> lines;
    for (const auto& gear : this->gear)
    {
        lines.push_back(gear->getGearInfo(dict));
        for (const auto& [cell, count] : gear->getCellList())
        {
            if (cell.isEmpty())
                continue;
            for (size_t i = 0; i < count; ++i)
                lines.push_back(indentation + cell.getCellInfo(dict));
        }
    }
    if (!lantern.isEmpty())
    {
        lines.push_back(dict.getTranslationFor("lantern"));
        lines.push_back(indentation + lantern.getCellInfo(dict));
    }
    size_t maxLineLength = 0;
    for (const auto& line : lines)
        if (line.length() > maxLineLength)
            maxLineLength = line.length();
    ++maxLineLength; // a single space at the end of the line
    auto skills = getSkills();
    skills.sort();
    auto uniqueSkills = getUniqueSkills();
    size_t count = 0;
    std::stringstream out;

    // adds a line to the outputstream and updates the length parameter
    auto addLine = [&out, &lines, &count, &maxLineLength, &dict]() {
        size_t length = 0;
        if (count < lines.size())
        {
            auto& line = lines[count];
            length = line.length();
            out << line;
        }
        out << std::string(maxLineLength - length, ' ') << "| ";
    };

    for (const auto& skill : skills)
    {
        addLine();
        out << skill.toString(dict);
        if (count < skills.size() - 1 || count < lines.size() - 1 || uniqueSkills.size() > 0)
            out << std::endl;
        ++count;
    }
    for (const auto& uniqueSkill : uniqueSkills)
    {
        addLine();
        out << uniqueSkill;
        if (count < lines.size() - 1 || count < skills.size() + uniqueSkill.size() - 1)
            out << std::endl;
        ++count;
    }
    for (; count < lines.size(); ++count)
    {
        out << lines[count];
        if (count < lines.size() - 1)
            out << std::endl;
    }
    return out.str();
}

bool Gear::ArmourSet::hasUniqueSkill() const
{
    for (auto gear : this->gear)
        if (gear->hasUniqueSkill())
            return true;
    return false;
}

std::vector<std::string> Gear::ArmourSet::getUniqueSkills() const
{
    std::vector<std::string> uniqueSkills;
    for (auto gear : this->gear)
    {
        auto unique = gear->getUniqueSkills();
        uniqueSkills.insert(uniqueSkills.end(), unique.begin(), unique.end());
    }
    return uniqueSkills;
}