#ifndef DAUNTLESS_ASS_ARMOURSET_HPP
#define DAUNTLESS_ASS_ARMOURSET_HPP

#include "gear/Armour.hpp"
#include "gear/Gear.hpp"
#include "gear/SkillType.hpp"
#include "gear/Weapon.hpp"

namespace Gear
{
class ArmourSet : public Gear
{
  public:
    ArmourSet(Armour head, Armour torso, Armour arms, Armour legs, Weapon weapon,
              Cell lantern = SkillType::Utility);
    ArmourSet(const ArmourSet &other);
    friend void swap(ArmourSet &first, ArmourSet &second);
    ArmourSet &operator=(ArmourSet other);

    /********** inherited from Gear ***************/
    // checks if the gear has free cell slots for a specific skill
    bool hasFreeCellSlotFor(SkillType type) const override;
    // adds a new Cell to the gear
    // returns if the cell could be inserted
    bool addCell(Cell cell) override;
    // returns the info of the gear as human readable string
    std::string getGearInfo(const Dictionary &dict) const override;
    // returns a list of all skills on the gear
    SkillList getSkills() const override;
    // returns the skill points for a specific skill on that gear
    int getSkillPointsFor(const std::string &skill) const override;
    // returns the cells on the gear
    CellList getCells() const override;
    virtual std::string getToolTip(const Dictionary &dict) const override { return ""; }

    const Armour &getHead() const;
    const Armour &getTorso() const;
    const Armour &getArms() const;
    const Armour &getLegs() const;
    const Weapon &getWeapon() const;

  private:
    Armour head, torso, legs, arms;
    Weapon weapon;
    std::vector<Gear *> gear;
    Cell lantern;
};
} // namespace Gear

#endif // !DAUNTLESS_ASS_ARMOURSET_HPP