#include "gear/Skill.hpp"
#include "gear/Armoury.hpp"
#include <sstream>

Gear::Skill::Skill() : Skill(NO_SKILL, 0) {}

Gear::Skill::Skill(size_t id, size_t skillPoints) : id_(id), skillPoints_(skillPoints) {}

size_t Gear::Skill::getId() const { return id_; }

size_t Gear::Skill::getSkillPoints() const { return skillPoints_; }

std::string Gear::Skill::toString(const Dictionary& dict, const Armoury& armoury) const
{
    std::stringstream ss;
    ss << dict.getTranslationFor(armoury.getSkillInfo(id_).getName()) << " +" << skillPoints_;
    return ss.str();
}

bool Gear::Skill::operator==(const Skill& other) const
{
    return skillPoints_ == other.skillPoints_ && id_ == other.id_;
}

bool Gear::Skill::operator<(const Skill& other) const
{
    if (skillPoints_ < other.skillPoints_)
        return true;
    else if (skillPoints_ == other.skillPoints_)
        return id_ < other.id_;
    else
        return false;
}

bool Gear::Skill::operator>(const Skill& other) const
{
    if (skillPoints_ > other.skillPoints_)
        return true;
    else if (skillPoints_ == other.skillPoints_)
        return id_ < other.id_;
    else
        return false;
}
