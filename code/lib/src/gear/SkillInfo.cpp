#include "gear/SkillInfo.hpp"
#include <stdexcept>

Gear::SkillInfo::SkillInfo(std::string name, std::string description, SkillType type,
                           std::vector<std::string> effects)
    : name(std::move(name)), description(std::move(description)), type(type),
      effects(std::move(effects))
{
}

const std::string &Gear::SkillInfo::getName() const { return name; }

Gear::SkillType Gear::SkillInfo::getType() const { return type; }

const std::string &Gear::SkillInfo::getDescription() const { return description; }

const std::string &Gear::SkillInfo::getEffectForLevel(size_t level) const
{
    if (level < 1)
        throw std::logic_error("no skill level smaller then 1 possible");
    if (level > effects.size())
        throw std::logic_error("This skill has a maximum level of " + std::to_string(getMaxLevel()));
    return effects[level - 1];
}

size_t Gear::SkillInfo::getMaxLevel() const { return effects.size(); }
