#include "gear/Armour.hpp"
#include <sstream>

Gear::Armour::Armour(ArmourType type, std::shared_ptr<GearInfo>& info, int level,
                     std::shared_ptr<std::vector<std::string>>& uniqueSkills, SkillType cellType,
                     const Skill& skill)
    : Gear(info, level, skill, uniqueSkills, {Cell(cellType)}), type(type)
{
}
