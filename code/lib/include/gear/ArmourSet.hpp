#ifndef DAUNTLESS_ASS_ARMOURSET_HPP
#define DAUNTLESS_ASS_ARMOURSET_HPP

#include "gear/Armour.hpp"
#include "gear/Gear.hpp"
#include "gear/SkillList.hpp"
#include "gear/SkillType.hpp"
#include "gear/Weapon.hpp"

namespace Gear
{
class Armoury;
class ArmourSet
{
  public:
    ArmourSet(const Armoury& armoury, const Armour& head, const Armour& torso, const Armour& arms,
              const Armour& legs, const Weapon& weapon, const Cell& lantern = SkillType::Utility);
    ArmourSet(const Armoury& armoury, Armour&& head, Armour&& torso, Armour&& arms, Armour&& legs,
              Weapon&& weapon, Cell&& lantern = SkillType::Utility);
    ArmourSet(const ArmourSet& other);
    ArmourSet(ArmourSet&& other);
    const ArmourSet& operator=(const ArmourSet& other) = delete;
    const ArmourSet& operator=(ArmourSet&& other) = delete;

    // checks if the gear has free cell slots for a specific skill
    bool hasFreeCellSlotFor(SkillType type) const;
    // adds a new Cell to the gear
    // returns if the cell could be inserted
    bool addCell(Cell cell);
    // returns a list of all skills on the gear
    SkillList getSkills() const;
    /**
     * check the skill points for a specific skill without cells
     * @param skillId The ID of the skill which shall be checked
     * @return Returns the skill points for the specified skill
     */
    size_t getSkillPointsFor(size_t skillId) const;
    // returns the cells on the gear
    CellList getCellList() const;
    virtual bool hasUniqueSkill() const;
    std::vector<std::string> getUniqueSkills() const;

    void setHead(const Armour& head);
    void setTorso(const Armour& torso);
    void setArms(const Armour& arms);
    void setLegs(const Armour& legs);
    void setWeapon(const Weapon& weapon);

    /**
     * gets the total number of cell slots for the specified type
     * @param type The skill type for which the cell slots are needed
     * @return The number of cell slot on the set for that type
     */
    size_t getCellSlotCountFor(SkillType type) const;
    /**
     * removes all cells from the set
     */
    void removeAllCells();

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

    std::string exportToText(const Dictionary& dict, const Armoury& armoury) const;

    std::string getHashIds(const Armoury& armoury) const;

  private:
    void init();
    void switchGear(const Gear& oldGear, const Gear& newGear);

    const Armoury& armoury_;
    Armour head_, torso_, legs_, arms_;
    Weapon weapon_;
    std::vector<Gear*> gear_;
    Cell lantern_;
    WantedSkillList innateSkills_;
    std::array<size_t, SkillType::MaxSkillType> cellSlotsPerType_;
};
} // namespace Gear

#endif // !DAUNTLESS_ASS_ARMOURSET_HPP