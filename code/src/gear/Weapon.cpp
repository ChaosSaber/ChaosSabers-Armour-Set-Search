#include "gear/Weapon.hpp"
#include <sstream>

Gear::Weapon::Weapon(WeaponType type, const std::string& name, const std::string& description,
                     int level, const Elements& elementalDamage,
                     const std::vector<std::string>& uniqueSkills, SkillType cellType1,
                     SkillType cellType2, const Skill& skill)
    : Gear(name, description, level, skill, uniqueSkills, {Cell(cellType1), Cell(cellType2)}),
      type(type), elementalDamage(elementalDamage)
{
}
