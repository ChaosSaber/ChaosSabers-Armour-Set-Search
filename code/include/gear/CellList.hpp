#ifndef DAUNTLESS_ASS_CELLLIST_HPP
#define DAUNTLESS_ASS_CELLLIST_HPP

#include "Dictionary.hpp"
#include "gear/Cell.hpp"
#include <array>
#include <unordered_map>

namespace Gear
{
class Armoury;

bool cellSorter(const std::pair<Cell, int>& lhs, const std::pair<Cell, int>& rhs);

class CellList
{
  public:
    CellList() {}
    CellList(const Cell& cell);
    CellList(const std::vector<Cell>& cells);

    void sort();
    /**
     *@return Returns the number of skills in the list
     */
    size_t size() const;

    std::vector<std::pair<Cell, int>>::iterator begin();
    std::vector<std::pair<Cell, int>>::const_iterator begin() const;
    std::vector<std::pair<Cell, int>>::iterator end();
    std::vector<std::pair<Cell, int>>::const_iterator end() const;
    /**
     * @param pos The position of the wanted object
     * @return A refrence to the wanted skill
     */
    const std::pair<Cell, int>& operator[](size_t pos) const;
    const CellList& operator+=(const Cell& cell);
    const CellList& operator+=(const CellList& cellList);
    const CellList& operator+=(const std::pair<Cell, int>& cellPair);
    friend CellList operator+(CellList lhs, const Cell& rhs);
    friend CellList operator+(CellList lhs, const CellList& rhs);
    const CellList& operator-=(const Cell& cell);
    const CellList& operator-=(const CellList& cellList);
    friend CellList operator-(CellList lhs, const Cell& rhs);
    friend CellList operator-(CellList lhs, const CellList& rhs);
    friend CellList operator*(size_t multiplicator, const Cell& cell);

  private:
    std::vector<std::pair<Cell, int>> cells;
};

/**
 * This is a cell list optimized for accessing a fixed skill ID.
 * In contrast to the CellList it does not have a map of cells with it counts, which needs to be
 * iterated all the time. This contains a fixed list which contains all skill IDs as primary index
 * and the cell level as secondary index. The access to it is allways constant.
 */
class AvailableCellList
{
  public:
    AvailableCellList(size_t size = 0);
    AvailableCellList(const Armoury& armoury);

    /**
     * searchs for the best cell of the given skill
     * @parameter skillId The skill ID for which the best cell is searched
     * return The highest cell level for that skill in the list
     */
    size_t getHighestAvailableCellLevel(size_t skillId) const;

    /**
     * checks if the cell list contains enough cells to reach the needed skill points
     * @param skillId The ID of the skill which shall be checked
     * @param neededSkillPoints The points which needs to be achieved
     * @return Returns true if enough cells are available, otherwise falls
     */
    bool hasEnoughCellsFor(size_t skillId, size_t neededSkillPoints) const;

    const AvailableCellList& operator+=(const CellList& lhs);
    const AvailableCellList& operator-=(const Cell& lhs);

  private:
    bool outOfBounds(size_t skillId) const;

    std::vector<std::array<size_t, 3>> cells_;
};
} // namespace Gear

#endif // !DAUNTLESS_ASS_CELLLIST_HPP