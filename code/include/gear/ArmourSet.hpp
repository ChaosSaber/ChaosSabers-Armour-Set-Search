#ifndef DAUNTLESS_ASS_ARMOURSET_HPP
#define DAUNTLESS_ASS_ARMOURSET_HPP

#include "gear/Armour.hpp"
#include "gear/Gear.hpp"
#include "gear/SkillType.hpp"
#include "gear/Weapon.hpp"

namespace Gear
{
class ArmourSet
{
  public:
    ArmourSet(const Armour& head, const Armour& torso, const Armour& arms, const Armour& legs,
              const Weapon& weapon, const Cell& lantern = SkillType::Utility);
    ArmourSet(Armour&& head, Armour&& torso, Armour&& arms, Armour&& legs, Weapon&& weapon,
              Cell&& lantern = SkillType::Utility);
    ArmourSet(const ArmourSet& other);
    //friend void swap(ArmourSet& first, ArmourSet& second);
    //ArmourSet& operator=(ArmourSet other);
    ArmourSet(ArmourSet&& other);

    // checks if the gear has free cell slots for a specific skill
    bool hasFreeCellSlotFor(SkillType type) const;
    // adds a new Cell to the gear
    // returns if the cell could be inserted
    bool addCell(Cell cell);
    // returns the info of the gear as human readable string
    std::string getGearInfo(const Dictionary& dict) const;
    // returns a list of all skills on the gear
    SkillList getSkills() const;
    // returns the skill points for a specific skill on that gear
    int getSkillPointsFor(const std::string& skill) const;
    // returns the cells on the gear
    CellList getCellList() const;
    virtual bool hasUniqueSkill() const;
    std::vector<std::string> getUniqueSkills() const;

    const Armour& getHead() const;
    const Armour& getTorso() const;
    const Armour& getArms() const;
    const Armour& getLegs() const;
    const Weapon& getWeapon() const;
    const Cell& getLantern() const;

    /**
     * gathers all additional skills which are not in the given skill list
     * @param wantedSkills The Skills which are wanted
     * @return Returns a Skill list with all skills which are not in the wanted skills list
     */
    SkillList getAdditionalSkills(const SkillList& wantedSkills) const;


    std::string exportToText(const Dictionary& dict) const;
    std::string exportToText2(const Dictionary& dict) const;

  private:
    void init();

    Armour head, torso, legs, arms;
    Weapon weapon;
    std::vector<Gear*> gear;
    Cell lantern;
};
} // namespace Gear

#endif // !DAUNTLESS_ASS_ARMOURSET_HPP