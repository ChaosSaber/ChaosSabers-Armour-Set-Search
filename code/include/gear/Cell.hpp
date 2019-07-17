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
    /**
     * check the skill points for a specific skill without cells
     * @param skillId The ID of the skill which shall be checked
     * @return Returns the skill points for the specified skill
     */
    size_t getSkillPointsFor(size_t skillId) const;
    /**
     * @return Returns the skill ID of the underlying skill
     */
    size_t getSkillId() const;
    bool isEmpty() const;
    const Skill& getSkill() const;
    /**
     * transforms the cell to an human readable string
     * @param dict A dictionary with all necessary translations
     * @param armoury The armoury with all the necessary skill data
     * @return Returns a human readable string
     */
    std::string getCellInfo(const Dictionary& dict, const Armoury& armoury) const;
    SkillType getCellType() const;

    //  operators
    bool operator==(const Cell& cell) const;
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
    std::size_t operator()(const Gear::Cell& cell) const
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