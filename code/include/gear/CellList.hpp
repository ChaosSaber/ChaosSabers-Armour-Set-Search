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

    std::string toString(const Dictionary &dict);
    bool hasEnoughCellsFor(const Skill &skill, int allreadyExistingSkillPoints) const;
    int getOptimalCellLevel(const Skill &skill, int existingSkillpoints) const;
    void sort();

    std::vector<std::pair<Cell, int>>::iterator begin();
    std::vector<std::pair<Cell, int>>::const_iterator begin() const;
    std::vector<std::pair<Cell, int>>::iterator end();
    std::vector<std::pair<Cell, int>>::const_iterator end() const;
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

    int getHighestCellLevel(std::unordered_map<int, int> levels, int maximumLevel) const;
};
} // namespace Gear

#endif // !DAUNTLESS_ASS_CELLLIST_HPP