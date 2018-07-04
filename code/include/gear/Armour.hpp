#ifndef DAUNTLESS_ASS_GEAR_ARMOUR_HPP
#define DAUNTLESS_ASS_GEAR_ARMOUR_HPP

#include "gear/Cell.hpp"
#include "gear/Elements.hpp"
#include "gear/Gear.hpp"
#include "gear/Skill.hpp"
#include "gear/SkillType.hpp"
#include <string>

namespace Gear
{

enum ArmourType { Head, Torso, Legs, Arms };

class Armour : public Gear
{
  public:
    Armour(ArmourType type, std::string name, unsigned int defense, Elements elementalResistance,
           Skill skill1, Skill skill2, SkillType cellType);
    unsigned int getDefense() const;
    const Elements &getElementalResistance() const;
    bool hasSkill(const std::string &skill) const;
    int getSkillPointsFor(const std::string &skill) const override;
    bool addCell(Cell cell) override;
    const Cell &getCell() const;
    bool hasFreeCellSlotFor(SkillType type) const override;
    // returns the info of the gear as human readable string
    std::string getGearInfo(const Dictionary &dict) const override;
    // returns a list of all skills on the gear
    SkillList getSkills() const override;
    CellList getCells() const override;
    const std::string &getName() const;
    virtual std::string getToolTip(const Dictionary &dict) const override;

  private:
    ArmourType type;
    std::string name;
    unsigned int defense;
    Elements elementalResistance;
    Skill skill1, skill2;
    Cell cell;
};

} // namespace Gear

#endif // !DAUNTLESS_ASS_GEAR_ARMOUR_HPP
