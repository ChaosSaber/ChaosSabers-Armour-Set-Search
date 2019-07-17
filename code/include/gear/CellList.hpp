#ifndef DAUNTLESS_ASS_CELLLIST_HPP
#define DAUNTLESS_ASS_CELLLIST_HPP

#include "Dictionary.hpp"
#include "gear/Cell.hpp"
#include <unordered_map>

namespace Gear
{
bool cellSorter(const std::pair<Cell, int> &lhs, const std::pair<Cell, int> &rhs);

class CellList
{
  public:
    CellList() {}
    CellList(const Cell &cell);
    CellList(const std::vector<Cell> &cells);

    bool hasEnoughCellsFor(const Skill& skill, size_t allreadyExistingSkillPoints) const;
    size_t getOptimalCellLevel(const Skill& skill, size_t existingSkillpoints) const;
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
    const CellList &operator+=(const Cell &cell);
    const CellList &operator+=(const CellList &cellList);
    friend CellList operator+(CellList lhs, const Cell &rhs);
    friend CellList operator+(CellList lhs, const CellList &rhs);
    const CellList &operator-=(const Cell &cell);
    const CellList &operator-=(const CellList &cellList);
    friend CellList operator-(CellList lhs, const Cell &rhs);
    friend CellList operator-(CellList lhs, const CellList &rhs);

  private:
    std::vector<std::pair<Cell, int>> cells;

    size_t getHighestCellLevel(std::unordered_map<size_t, size_t> levels,
                               size_t maximumLevel) const;
};
} // namespace Gear

#endif // !DAUNTLESS_ASS_CELLLIST_HPP