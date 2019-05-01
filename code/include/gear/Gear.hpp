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
    Gear(const std::string& name, const std::string& description, int level, const Skill& skill,
         const std::vector<std::string>& uniqueSkills, const std::vector<Cell>& cells);
    Gear(std::string&& name, std::string&& description, int level, Skill&& skill,
         std::vector<std::string>&& uniqueSkills, std::vector<Cell>&& cells);

    //Gear(const Gear& other);
    //Gear(Gear&& other);

    // checks if the gear has free cell slots for a specific skill
    bool hasFreeCellSlotFor(SkillType type) const;
    // adds a new Cell to the gear
    // returns if the cell could be inserted
    bool addCell(const Cell& cell);
    // returns the cells on the gear
    CellList getCellList() const;

    const std::string& getName() const;
    int Gear::Gear::getLevel() const;
    const std::string& Gear::Gear::getDescription() const;

    bool hasSkill(const std::string& skill) const;
    // returns the skill points for a specific skill on that gear
    int getSkillPointsFor(const std::string& skill) const;
    // returns a list of all skills on the gear
    SkillList getSkills() const;
    // returns a list of all unique skills on the gear
    const std::vector<std::string>& getUniqueSkills() const;
    bool hasUniqueSkill() const;
    // returns the info of the gear as human readable string
    std::string getGearInfo(const Dictionary& dict) const;

    virtual std::string getToolTip(const Dictionary& dict) const;

  private:
    std::string name_;
    std::string description_;
    int level_;
    // exotics have only unique skills, therefore we need a pointer
    Skill skill_;
    std::vector<std::string> uniqueSkills_;
    std::vector<Cell> cells_;
};
} // namespace Gear

#endif // ! DAUNTLESS_ASS_GEAR_GEAR_HPP
