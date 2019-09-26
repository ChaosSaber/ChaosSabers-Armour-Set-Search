#ifndef DAUNTLESS_ASS_GEAR_SKILLTYPE_HPP
#define DAUNTLESS_ASS_GEAR_SKILLTYPE_HPP

#include <string>

namespace Gear
{

enum SkillType { None = 0, Power, Technique, Mobility, Defense, Utility, MaxSkillType };

std::string SkillTypeToStringKey(SkillType type);

std::string cellSlotToStringKey(SkillType type);
std::string FreeCellSlotToStringKey(SkillType type);
} // namespace Gear

#endif // !DAUNTLESS_ASS_GEAR_SKILLTYPE_HPP