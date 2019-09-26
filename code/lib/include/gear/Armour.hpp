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
    Armour(ArmourType type, std::shared_ptr<GearInfo>& info, int level,
           std::shared_ptr<std::vector<std::string>>& uniqueSkills, SkillType cellType,
           const Skill& skills = Skill());

  private:
    ArmourType type;
};

} // namespace Gear

#endif // !DAUNTLESS_ASS_GEAR_ARMOUR_HPP
