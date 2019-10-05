#ifndef DAUNTLESS_ASS_GEAR_SKILLINFO_HPP
#define DAUNTLESS_ASS_GEAR_SKILLINFO_HPP

#include "gear/SkillType.hpp"
#include <string>
#include <vector>

namespace Gear
{
class SkillInfo
{
  public:
    SkillInfo(std::string name, std::string description, SkillType type,
              std::vector<std::string> effects);
    const std::string &getName() const;
    SkillType getType() const;
    const std::string &getDescription() const;
    const std::string &getEffectForLevel(size_t level) const;
    size_t getMaxLevel() const;

  private:
    std::string name;
    std::string description;
    SkillType type;
    std::vector<std::string> effects;
};
} // namespace Gear

#endif // DAUNTLESS_ASS_GEAR_SKILLINFO_HPP