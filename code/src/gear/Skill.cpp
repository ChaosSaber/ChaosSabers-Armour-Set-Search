#include "gear/Skill.hpp"
#include <sstream>

Gear::Skill::Skill() : Skill("", 0, false) {}

Gear::Skill::Skill(std::string name, int skillPoints, bool unique)
    : name(std::move(name)), skillPoints(skillPoints), unique(unique)
{
}

const std::string &Gear::Skill::getName() const { return name; }

int Gear::Skill::getSkillPoints() const { return skillPoints; }

std::string Gear::Skill::toString(const Dictionary &dict) const
{
    std::stringstream ss;
    ss << dict.getTranslationFor(name) ;
    if (!unique)
        ss << " +" << skillPoints;
    return ss.str();
}

bool Gear::Skill::isUnique() const { return unique; }
