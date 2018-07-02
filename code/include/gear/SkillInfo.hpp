#ifndef DAUNTLESS_ASS_GEAR_SKILLINFO_HPP
#define DAUNTLESS_ASS_GEAR_SKILLINFO_HPP

#include "gear/SkillType.hpp"
#include <string>

namespace Gear
{
class SkillInfo
{
  public:
    SkillInfo(std::string name, SkillType type);
    const std::string &getName() const;
    SkillType getType() const;

  private:
    std::string name;
    SkillType type;
};
} // namespace Gear

#endif // DAUNTLESS_ASS_GEAR_SKILLINFO_HPP