#ifndef DAUNTLESS_ASS_GEAR_GEAR_HPP
#define DAUNTLESS_ASS_GEAR_GEAR_HPP

#include "Dictionary.hpp"
#include "gear/Cell.hpp"
#include "gear/CellList.hpp"
#include "gear/SkillList.hpp"
#include <string>

namespace Gear
{
class Gear
{
  public:
    // checks if the gear has free cell slots for a specific skill
    virtual bool hasFreeCellSlotFor(SkillType type) const = 0;
    // adds a new Cell to the gear
    // returns if the cell could be inserted
    virtual bool addCell(Cell cell) = 0;
    // returns the info of the gear as human readable string
    virtual std::string getGearInfo(const Dictionary &dict) const = 0;
    // returns a list of all skills on the gear
    virtual SkillList getSkills() const = 0;
    // returns the skill points for a specific skill on that gear
    virtual int getSkillPointsFor(const std::string &skill) const = 0;
    // returns the cells on the gear
    virtual CellList getCells() const = 0;
};
} // namespace Gear

#endif // ! DAUNTLESS_ASS_GEAR_GEAR_HPP
