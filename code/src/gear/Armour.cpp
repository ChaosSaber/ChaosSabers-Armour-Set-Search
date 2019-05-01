#include "gear/Armour.hpp"
#include <sstream>

Gear::Armour::Armour(ArmourType type, const std::string& name, const std::string& description,
                     int level, const Elements& elementalResistance,
                     const std::vector<std::string>& uniqueSkills, SkillType cellType,
                     const Skill& skill)
    : Gear(name, description, level, skill, uniqueSkills, {Cell(cellType)}), type(type),
      elementalResistance_(elementalResistance)
{
}

const Gear::Elements& Gear::Armour::getElementalResistance() const { return elementalResistance_; }
