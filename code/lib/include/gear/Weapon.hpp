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
    Weapon(WeaponType type, std::shared_ptr<GearInfo>& info, int level,
           std::shared_ptr<std::vector<std::string>>& uniqueSkills, SkillType cellType1,
           SkillType cellType2, const Skill& skill = Skill());

    /**
     * @return Returns the type of the weapon.
     */
    WeaponType getType() const;

  private:
    WeaponType type;
};
} // namespace Gear

#endif // !DAUNTLESS_ASS_GEAR_WEAPON_HPP