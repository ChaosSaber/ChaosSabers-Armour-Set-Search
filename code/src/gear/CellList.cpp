#include "gear/CellList.hpp"
#include <algorithm>
#include <sstream>

Gear::CellList::CellList(const Cell& cell) { *this += cell; }

Gear::CellList::CellList(const std::vector<Cell>& cells)
{
    for (auto& cell : cells)
        *this += cell;
}

size_t Gear::CellList::getHighestAvailableCellLevel(const Skill& skill) const
{
    size_t highest = 0;
    for (const auto& cell : cells)
        if (cell.first.getSkillId() == skill.getId())
            if (cell.first.getSkill().getSkillPoints() > highest)
                highest = cell.first.getSkill().getSkillPoints(); 
    return highest;
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
            return lhs.first.getSkillId() > rhs.first.getSkillId();
        else
            return false;
    }
    else
        return false;
}

bool Gear::CellList::hasEnoughCellsFor(const Skill& skill, size_t allreadyExistingSkillPoints) const
{
    if (allreadyExistingSkillPoints > skill.getSkillPoints())
        return true;
    size_t sum = 0;
    for (const auto& cell : cells)
        if (cell.first.getSkillId() == skill.getId())
            sum += cell.second * cell.first.getSkill().getSkillPoints();
    return sum >= skill.getSkillPoints() - allreadyExistingSkillPoints;
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

const Gear::CellList& Gear::CellList::operator+=(const std::pair<Cell, int>& cellPair)
{
    for (auto& cellCount : cells)
        if (cellCount.first == cellPair.first)
        {
            cellCount.second+=cellPair.second;
            return *this;
        }
    cells.push_back(cellPair);
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

Gear::CellList Gear::operator*(size_t multiplicator, const Cell& cell)
{
    CellList cells;
    cells += {cell, multiplicator};
    return cells;
}
