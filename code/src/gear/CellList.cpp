#include "gear/CellList.hpp"
#include "gear/Armoury.hpp"
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
            return lhs.first.getSkillId() > rhs.first.getSkillId();
        else
            return false;
    }
    else
        return false;
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
            cellCount.second += cellPair.second;
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

Gear::AvailableCellList::AvailableCellList(size_t size) : cells_(size, CellArray()) {}

Gear::AvailableCellList::AvailableCellList(const Armoury& armoury)
    : AvailableCellList(armoury.getSkillInfos().size())
{
}

size_t Gear::AvailableCellList::getHighestAvailableCellLevel(size_t skillId) const
{
    return cells_[skillId].getHighestAvailableCellLevel();
}

bool Gear::AvailableCellList::hasEnoughCellsFor(size_t skillId, size_t neededSkillPoints) const
{
    return cells_[skillId].getTotalSkillcount() >= neededSkillPoints;
}

size_t Gear::AvailableCellList::cellsNeededForSkillCount(size_t skillId, size_t skillCount) const
{
    return cells_[skillId].cellsNeededForSkillCount(skillCount);
}

const std::vector<size_t>&
Gear::AvailableCellList::getCellLevelsForSkillCount(size_t skillId, size_t skillCount) const
{
    return cells_[skillId].getCellLevelsForSkillCount(skillCount);
}

const Gear::AvailableCellList& Gear::AvailableCellList::operator+=(const CellList& lhs)
{
    for (const auto& cell : lhs)
    {
        if (cell.first.isEmpty())
            continue;
        cells_[cell.first.getSkillId()].add(cell.first.getSkill().getSkillPoints(), cell.second);
    }
    return *this;
}

const Gear::AvailableCellList& Gear::AvailableCellList::operator-=(const Cell& lhs)
{
    cells_[lhs.getSkillId()].remove(lhs.getSkill().getSkillPoints());
    return *this;
}

size_t Gear::AvailableCellList::CellArray::getHighestAvailableCellLevel() const
{
    for (size_t i = 3; i > 0; --i)
        if (cells_[i - 1] > 0)
            return i;
    return 0;
}

size_t Gear::AvailableCellList::CellArray::getTotalSkillcount() const { return totalSkillCount_; }

size_t Gear::AvailableCellList::CellArray::cellsNeededForSkillCount(size_t skillCount) const
{
    return cellsForSkillCount_[skillCount - 1].size();
}

const std::vector<size_t>&
Gear::AvailableCellList::CellArray::getCellLevelsForSkillCount(size_t skillCount) const
{
    return cellsForSkillCount_[skillCount - 1];
}

void Gear::AvailableCellList::CellArray::add(size_t cellLevel, size_t amount)
{
    cells_[cellLevel - 1] += amount;
    totalSkillCount_ += cellLevel * amount;
    for (size_t i = 1; i <= cellsForSkillCount_.size(); ++i)
    {
        cellsForSkillCount_[i - 1].clear();
        if (totalSkillCount_ < i)
            continue;
        auto copy = cells_;
        size_t currentSkillCount = 0;
        for (size_t k = 3; k > 0 && currentSkillCount < i; ++k)
        {
            while (copy[k - 1] > 0 && currentSkillCount < i)
            {
                currentSkillCount += k;
                --copy[k - 1];
                cellsForSkillCount_[i - 1].push_back(k);
            }
        }
    }
}

void Gear::AvailableCellList::CellArray::remove(size_t cellLevel)
{
    --cells_[cellLevel - 1];
    totalSkillCount_ -= cellLevel;
}
