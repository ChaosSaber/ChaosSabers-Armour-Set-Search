#ifndef DAUNTLESS_ASS_GEAR_WEAPON_HPP
#define DAUNTLESS_ASS_GEAR_WEAPON_HPP

#include "gear/Cell.hpp"
#include "gear/Elements.hpp"
#include "gear/Gear.hpp"
#include "gear/Skill.hpp"
#include "gear/SkillType.hpp"
#include <string>

namespace Gear
{

enum WeaponType { Sword, Hammer, Chainblades, Axe, Pike, Reapeater };

class Weapon : public Gear
{
  public:
    Weapon(WeaponType type, const std::string& name, const std::string& description, int level,
           const Elements& elementalDamage, const std::vector<std::string>& uniqueSkills,
           SkillType cellType1, SkillType cellType2, const Skill& skill = Skill());

  private:
    WeaponType type;
    Elements elementalDamage;
};
} // namespace Gear

#endif // !DAUNTLESS_ASS_GEAR_WEAPON_HPP