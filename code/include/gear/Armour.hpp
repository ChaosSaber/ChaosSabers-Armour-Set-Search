#ifndef DAUNTLESS_ASS_GEAR_ARMOUR_HPP
#define DAUNTLESS_ASS_GEAR_ARMOUR_HPP

#include "gear/Cell.hpp"
#include "gear/Elements.hpp"
#include "gear/Gear.hpp"
#include "gear/Rarity.hpp"
#include "gear/Skill.hpp"
#include "gear/SkillType.hpp"
#include <string>

namespace Gear
{

enum ArmourType { Head, Torso, Legs, Arms };

class Armour : public Gear
{
  public:
    Armour(ArmourType type, std::string name, std::string description, int tier,
           unsigned int minDefense, unsigned int maxDefense, Elements elementalResistance,
           std::vector<Skill> skills, std::vector<std::string> uniqueSkills, SkillType cellType,
           Rarity rarity);
    Armour(ArmourType type, std::string name, std::string description, std::vector<int> tiers,
           unsigned int minDefense, unsigned int maxDefense, Elements elementalResistance,
           std::vector<Skill> skills, std::vector<std::string> uniqueSkills, SkillType cellType,
           Rarity rarity);

    unsigned int getMinDefense() const;
    unsigned int getMaxDefense() const;
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
    const std::vector<std::string> &getUniqueSkills() const override;
    CellList getCells() const override;
    virtual std::string getToolTip(const Dictionary &dict) const override;
    virtual bool hasUniqueSkill() const override;
    bool isExotic() const;

  private:
    ArmourType type;
    std::string description;
    unsigned int minDefense, maxDefense;
    Elements elementalResistance;
    std::vector<Skill> skills;
    Cell cell;
    std::vector<std::string> uniqueSkills;
    Rarity rarity;
};

} // namespace Gear

#endif // !DAUNTLESS_ASS_GEAR_ARMOUR_HPP
