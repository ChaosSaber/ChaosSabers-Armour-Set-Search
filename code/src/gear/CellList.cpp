#include "gear/CellList.hpp"
#include <algorithm>
#include <sstream>

Gear::CellList::CellList(const Cell& cell) { *this += cell; }

Gear::CellList::CellList(const std::vector<Cell>& cells)
{
    for (auto& cell : cells)
        *this += cell;
}

bool Gear::cellSorter(const std::pair<Cell, int>& lhs, const std::pair<Cell, int>& rhs)
{
    if (lhs.first.getSkill().getSkillPoints() > rhs.first.getSkill().getSkillPoints())
        return true;
    else if (lhs.first.getSkill().getSkillPoints() == rhs.first.getSkill().getSkillPoints())
    {
        if (lhs.second > rhs.second)
            return true;
        else if (lhs.second == rhs.second)
            return lhs.first.getSkillName() > rhs.first.getSkillName();
        else
            return false;
    }
    else
        return false;
}

std::string Gear::CellList::toString(const Dictionary& dict)
{
    std::stringstream ss;
    sort();
    for (const auto& cell : cells)
    {
        if (cell.second <= 0)
            continue;
        ss << cell.second << "x " << cell.first.getCellInfo(dict) << std::endl;
    }
    return ss.str();
}

bool Gear::CellList::hasEnoughCellsFor(const Skill& skill, int allreadyExistingSkillPoints) const
{
    int sum = 0;
    for (const auto& cell : cells)
        if (cell.first.getSkillName() == skill.getName())
            sum += cell.second * cell.first.getSkill().getSkillPoints();
    return sum > skill.getSkillPoints() - allreadyExistingSkillPoints;
}

int Gear::CellList::getOptimalCellLevel(const Skill& skill, int existingSkillpoints) const
{
    std::unordered_map<int, int> levels;
    for (const auto& cell : cells)
        if (cell.first.getSkillName() == skill.getName())
            levels[cell.first.getSkill().getSkillPoints()] = cell.second;
    switch (skill.getSkillPoints() - existingSkillpoints)
    {
    case 6:
        // 3 level 2 cells are cheaper then one of each cell level
        if (levels[3] < 2 && levels[2] >= 3)
            return 2;
    case 5: return getHighestCellLevel(levels, skill.getSkillPoints() - existingSkillpoints); break;
    case 4:
        // for 4 Skill points we need anyway 2 cells, so we try to use 2 level 2 cells instead of a
        // level 3 and level 1 cell
        if (levels[2] >= 2)
        {
            if (skill.getSkillPoints() < 6) // since we need two cells anyway we, look if we can
                                            // improve the skill, if the user did't searched for +6
                return getHighestCellLevel(levels, 6 - existingSkillpoints);
            return 2;
        }
        return getHighestCellLevel(levels, 4);
        break;
    case 3: return getHighestCellLevel(levels, 3); break;
    case 2:
    case 1:
        if (skill.getSkillPoints() <
            6) // we need a cell anyway, so we can look if we can improve the skill any further
            return getHighestCellLevel(levels, 6 - existingSkillpoints);
        return getHighestCellLevel(levels, skill.getSkillPoints() - existingSkillpoints);
        break;
    default: return 0; break;
    }
}

int Gear::CellList::getHighestCellLevel(std::unordered_map<int, int> levels, int maximumLevel) const
{
    int highest = 0;
    for (const auto& level : levels)
        if (level.first <= maximumLevel && level.second > 0 && level.first > highest)
            highest = level.first;
    return highest;
}

void Gear::CellList::sort() { std::sort(cells.begin(), cells.end(), cellSorter); }

size_t Gear::CellList::size() const { return cells.size(); }

std::vector<std::pair<Gear::Cell, int>>::iterator Gear::CellList::begin() { return cells.begin(); }

std::vector<std::pair<Gear::Cell, int>>::const_iterator Gear::CellList::begin() const
{
    return cells.cbegin();
}

std::vector<std::pair<Gear::Cell, int>>::iterator Gear::CellList::end() { return cells.end(); }

std::vector<std::pair<Gear::Cell, int>>::const_iterator Gear::CellList::end() const
{
    return cells.cend();
}

const std::pair<Gear::Cell, int>& Gear::CellList::operator[](size_t pos) const
{
    return cells[pos];
}

const Gear::CellList& Gear::CellList::operator+=(const Cell& cell)
{
    for (auto& cellCount : cells)
        if (cellCount.first == cell)
        {
            ++cellCount.second;
            return *this;
        }
    cells.push_back({cell, 1});
    return *this;
}

const Gear::CellList& Gear::CellList::operator+=(const CellList& cellList)
{
    for (const auto& cell : cellList.cells)
        for (size_t i = 0; i < cell.second; ++i)
            *this += cell.first;
    return *this;
}

Gear::CellList Gear::operator+(CellList lhs, const Cell& rhs) { return lhs += rhs; }

Gear::CellList Gear::operator+(CellList lhs, const CellList& rhs) { return lhs += rhs; }

const Gear::CellList& Gear::CellList::operator-=(const Cell& cell)
{
    for (auto& cellCount : cells)
        if (cellCount.first == cell)
        {
            --cellCount.second;
            // remove cells with count 0?
            return *this;
        }
    // should never happen
    return *this;
}

const Gear::CellList& Gear::CellList::operator-=(const CellList& cellList)
{
    for (const auto& cell : cellList.cells)
        for (size_t i = 0; i < cell.second; ++i)
            *this -= cell.first;
    return *this;
}

Gear::CellList Gear::operator-(CellList lhs, const Cell& rhs) { return lhs -= rhs; }

Gear::CellList Gear::operator-(CellList lhs, const CellList& rhs) { return lhs -= rhs; }
