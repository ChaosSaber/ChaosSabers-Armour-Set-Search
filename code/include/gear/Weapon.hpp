#ifndef DAUNTLESS_ASS_GEAR_WEAPON_HPP
#define DAUNTLESS_ASS_GEAR_WEAPON_HPP

#include "gear/Cell.hpp"
#include "gear/Elements.hpp"
#include "gear/Gear.hpp"
#include "gear/Rarity.hpp"
#include "gear/Skill.hpp"
#include "gear/SkillType.hpp"
#include <string>

namespace Gear
{

enum WeaponType { Sword, Hammer, Chainblades, Axe, Pike };

class Weapon : public Gear
{
  public:
    Weapon(WeaponType type, std::string name, std::string description, int tier,
           unsigned int minDamage, unsigned int maxDamage, Elements elementalDamage,
           std::vector<Skill> skills, std::vector<std::string> uniqueSkills, SkillType cellType1,
           SkillType cellType2, Rarity rarity);

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
    const std::string &getName() const;
    virtual std::string getToolTip(const Dictionary &dict) const override;
    virtual bool hasUniqueSkill() const override;
    std::vector<std::string> getUniqueSkills() const override;
    bool isExotic() const;
    virtual int getTier() const override;

    const Cell &getCell1() const;
    const Cell &getCell2() const;

  private:
    WeaponType type;
    int tier;
    std::string name;
    std::string description;
    unsigned int minDamage, maxDamage;
    Elements elementalDamage;
    std::vector<Skill> skills;
    std::vector<std::string> uniqueSkills;
    Cell cell1, cell2;
    Rarity rarity;
};
} // namespace Gear

#endif // !DAUNTLESS_ASS_GEAR_WEAPON_HPP