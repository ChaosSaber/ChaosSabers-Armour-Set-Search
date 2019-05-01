#ifndef DAUNTLESS_ASS_GEAR_ARMOUR_HPP
#define DAUNTLESS_ASS_GEAR_ARMOUR_HPP

#include "gear/Cell.hpp"
#include "gear/Elements.hpp"
#include "gear/Gear.hpp"
#include "gear/Skill.hpp"
#include "gear/SkillType.hpp"
#include <memory>
#include <string>

namespace Gear
{

enum ArmourType { Head, Torso, Legs, Arms };

class Armour : public Gear
{
  public:
    Armour(ArmourType type, const std::string& name, const std::string& description, int level,
           const Elements& elementalResistance, const std::vector<std::string>& uniqueSkills,
           SkillType cellType, const Skill& skills = Skill());

    const Elements& getElementalResistance() const;

  private:
    ArmourType type;
    Elements elementalResistance_;
};

} // namespace Gear

#endif // !DAUNTLESS_ASS_GEAR_ARMOUR_HPP
