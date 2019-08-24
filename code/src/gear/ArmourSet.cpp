#include "gear/ArmourSet.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>

Gear::ArmourSet::ArmourSet(const Armour& head, const Armour& torso, const Armour& arms,
                           const Armour& legs, const Weapon& weapon, const Cell& lantern)
    : head_(head), torso_(torso), legs_(legs), arms_(arms), weapon_(weapon), lantern_(lantern)
{
    init();
}

Gear::ArmourSet::ArmourSet(Armour&& head, Armour&& torso, Armour&& arms, Armour&& legs,
                           Weapon&& weapon, Cell&& lantern)
    : head_(std::move(head)), torso_(std::move(torso)), legs_(std::move(legs)),
      arms_(std::move(arms)), weapon_(std::move(weapon)), lantern_(std::move(lantern))
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
}

Gear::ArmourSet::ArmourSet(const ArmourSet& other)
    : ArmourSet(other.head_, other.torso_, other.arms_, other.legs_, other.weapon_, other.lantern_)
{
}

const Gear::ArmourSet& Gear::ArmourSet::operator=(ArmourSet&& other)
{
    head_ = std::move(other.head_);
    torso_ = std::move(other.torso_);
    arms_ = std::move(other.arms_);
    legs_ = std::move(other.legs_);
    weapon_ = std::move(other.weapon_);
    lantern_ = std::move(other.lantern_);
    return *this;
}

Gear::ArmourSet::ArmourSet(ArmourSet&& other)
    : ArmourSet(std::move(other.head_), std::move(other.torso_), std::move(other.arms_),
                std::move(other.legs_), std::move(other.weapon_), std::move(other.lantern_))
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
    size_t sum = 0;
    sum += lantern_.getSkillPointsFor(skillId);
    for (const auto part : gear_)
        sum += part->getSkillPointsFor(skillId);
    return sum;
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

std::string Gear::ArmourSet::exportToText2(const Dictionary& dict, const Armoury& armoury) const
{
    constexpr auto indentation = "    ";
    std::vector<std::string> lines;
    for (const auto& gear : this->gear_)
    {
        lines.push_back(gear->getGearInfo(dict));
        for (const auto& [cell, count] : gear->getCellList())
        {
            if (cell.isEmpty())
                continue;
            for (size_t i = 0; i < count; ++i)
                lines.push_back(indentation + cell.getCellInfo(dict, armoury));
        }
    }
    if (!lantern_.isEmpty())
    {
        lines.push_back(dict.getTranslationFor("lantern"));
        lines.push_back(indentation + lantern_.getCellInfo(dict, armoury));
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
        out << skill.toString(dict, armoury);
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

std::string Gear::ArmourSet::exportToText3(const Dictionary& dict, const Armoury& armoury) const
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

    size_t maxColoumnLength = std::max({maxGearLength, maxCellLength, maxSkillLength}) + 1;
    size_t lineWidth = 2 * maxColoumnLength;
    ++maxColoumnLength; // we add an additional space before the second coloumn as coloumnseparator

    // Weapon
    auto weaponStr = weapon_.getGearInfo(dict);
    text << weaponStr << std::endl;
    // Armour
    auto lineAdder = [&text, maxColoumnLength](const std::string& armour1,
                                               const std::string& armour2) {
        text << armour1 << std::string(maxColoumnLength - armour1.length(), ' ') << armour2
             << std::endl;
    };
    lineAdder(head_.getGearInfo(dict), torso_.getGearInfo(dict));
    lineAdder(arms_.getGearInfo(dict), legs_.getGearInfo(dict));
    text << std::string(lineWidth, '-') << std::endl;
    // cells
    auto cellPairToString = [&dict, &armoury](const std::pair<Cell, int>& cell) {
        std::stringstream str;
        str << cell.second << "x " << cell.first.getCellInfo(dict, armoury);
        return str.str();
    };
    for (size_t i = 0; i < cells.size(); i += 2)
    {
        auto cell1Str = cellPairToString(cells[i]);
        if (i + 1 < cells.size())
        {
            auto cell2Str = cellPairToString(cells[i + 1]);
            lineAdder(cell1Str, cell2Str);
        }
        else
        {
            text << cell1Str << std::endl;
        }
    }
    // skills
    text << std::string(lineWidth, '-') << std::endl;
    for (size_t i = 0; i < skills.size(); i += 2)
    {
        if (i + 1 < skills.size())
        {
            lineAdder(skills[i].toString(dict, armoury), skills[i + 1].toString(dict, armoury));
        }
        else
        {
            text << skills[i].toString(dict, armoury) << std::endl;
        }
    }
    // unique skills
    auto uniqueSkills = getUniqueSkills();
    if (!uniqueSkills.empty())
    {
        text << std::string(lineWidth, '-') << std::endl;
        for (const auto& skill : uniqueSkills)
            text << skill << std::endl;
    }
    return text.str();
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

void Gear::ArmourSet::setHead(const Armour& head) { head_ = head; }

void Gear::ArmourSet::setTorso(const Armour& torso) { torso_ = torso; }

void Gear::ArmourSet::setArms(const Armour& arms) { arms_ = arms; }

void Gear::ArmourSet::setLegs(const Armour& legs) { legs_ = legs; }

void Gear::ArmourSet::setWeapon(const Weapon& weapon) { weapon_ = weapon; }

Gear::CellList Gear::ArmourSet::removeCells(const Skill& skill)
{
    CellList cells;
    for (auto& gear : gear_)
        cells += gear->removeCells(skill);
    if (!lantern_.isEmpty() && lantern_.getSkillId() == skill.getId())
    {
        cells += lantern_;
        lantern_ = Cell(lantern_.getCellType());
    }
    return cells;
}
