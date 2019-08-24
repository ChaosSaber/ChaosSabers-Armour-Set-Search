#ifndef DAUNTLESS_ASS_CELLLIST_HPP
#define DAUNTLESS_ASS_CELLLIST_HPP

#include "Dictionary.hpp"
#include "gear/Cell.hpp"
#include <unordered_map>

namespace Gear
{
bool cellSorter(const std::pair<Cell, int>& lhs, const std::pair<Cell, int>& rhs);

class CellList
{
  public:
    CellList() {}
    CellList(const Cell& cell);
    CellList(const std::vector<Cell>& cells);

    /**
    * searchs for the best cell of the given skill
    * @parameter skill The skill for which the best cell is searched
    * return The highest cell level for that skill in the list
    */
    size_t getHighestAvailableCellLevel(const Skill& skill) const;

    bool hasEnoughCellsFor(const Skill& skill, size_t allreadyExistingSkillPoints) const;
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
     * @param pos Th position of the wanted object
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
} // namespace Gear

#endif // !DAUNTLESS_ASS_CELLLIST_HPP