#include "gear/Skill.hpp"
#include <sstream>

Gear::Skill::Skill() : Skill("", 0) {}

Gear::Skill::Skill(std::string name, int skillPoints)
    : name(std::move(name)), skillPoints(skillPoints)
{
}

const std::string &Gear::Skill::getName() const { return name; }

int Gear::Skill::getSkillPoints() const { return skillPoints; }

std::string Gear::Skill::toString(const Dictionary &dict) const
{
    std::stringstream ss;
    ss << dict.getTranslationFor(name) << " +" << skillPoints;
    return ss.str();
}