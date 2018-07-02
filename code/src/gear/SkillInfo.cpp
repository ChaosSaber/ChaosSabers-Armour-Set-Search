#include "gear/SkillInfo.hpp"

Gear::SkillInfo::SkillInfo(std::string name, SkillType type) : name(std::move(name)), type(type) {}

const std::string &Gear::SkillInfo::getName() const { return name; }

Gear::SkillType Gear::SkillInfo::getType() const { return type; }