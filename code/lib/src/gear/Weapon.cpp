#include "gear/Weapon.hpp"
#include <sstream>

Gear::Weapon::Weapon(WeaponType type, std::shared_ptr<GearInfo>& info, int level,
                     std::shared_ptr<std::vector<std::string>>& uniqueSkills, SkillType cellType1,
                     SkillType cellType2, const Skill& skill)
    : Gear(info, level, skill, uniqueSkills, {Cell(cellType1), Cell(cellType2)}), type(type)
{
}

Gear::WeaponType Gear::Weapon::getType() const { return type; }
