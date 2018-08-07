#include "gear/SkillInfo.hpp"

Gear::SkillInfo::SkillInfo(std::string name, std::string description, SkillType type,
                           std::vector<std::string> effects)
    : name(std::move(name)), description(std::move(description)), type(type),
      effects(std::move(effects))
{
}

const std::string &Gear::SkillInfo::getName() const { return name; }

Gear::SkillType Gear::SkillInfo::getType() const { return type; }

const std::string &Gear::SkillInfo::getDescription() const { return description; }

const std::string &Gear::SkillInfo::getEffectForLevel(int level) const
{
    if (level < 1)
        throw std::logic_error("no skill level smaller then 1 possible");
    if (level > effects.size())
        return effects.back();
    return effects[level - 1];
}
