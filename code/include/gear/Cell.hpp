#ifndef DAUNTLESS_ASS_GEAR_CELL_HPP
#define DAUNTLESS_ASS_GEAR_CELL_HPP

#include "Dictionary.hpp"
#include "gear/Skill.hpp"
#include "gear/SkillType.hpp"
#include <functional>

namespace Gear
{
class CellList;

class Cell
{
  public:
    Cell(SkillType type);
    Cell(Skill skill, SkillType type);
    int getSkillPointsFor(const std::string &skill) const;
    const std::string &getSkillName() const;
    bool isEmpty() const;
    const Skill &getSkill() const;
    std::string getCellInfo(const Dictionary &dict) const;
    SkillType getCellType() const;

    //  operators
    bool operator==(const Cell &cell) const;
    CellList operator*(size_t multiplicator) const;

  private:
    Skill skill;
    bool empty;
    SkillType type;
};
} // namespace Gear

namespace std
{

template <> struct hash<Gear::Cell>
{
    std::size_t operator()(const Gear::Cell &cell) const
    {
        using std::hash;
        using std::size_t;
        using std::string;

        // Compute individual hash values for first,
        // second and third and combine them using XOR
        // and bit shifting:

        return hash<Gear::Skill>()(cell.getSkill()) ^ (hash<int>()((int)cell.getCellType()) << 1);
    }
};
} // namespace std
#endif //  DAUNTLESS_ASS_GEAR_CELL_HPP