#ifndef DAUNTLESS_ASS_GEAR_SKILL_HPP
#define DAUNTLESS_ASS_GEAR_SKILL_HPP

#include "Dictionary.hpp"
#include <functional>
#include <string>

namespace Gear
{
class Skill
{
  public:
    Skill();
    Skill(std::string name, int skillPoints);

    const std::string& getName() const;
    int getSkillPoints() const;
    std::string toString(const Dictionary& dict) const;
    friend class SkillList;

    bool operator==(const Skill& other) const;
    bool operator<(const Skill& other) const;
    bool operator>(const Skill& other) const;

  private:
    std::string name;
    int skillPoints;
};
} // namespace Gear

namespace std
{

template <> struct hash<Gear::Skill>
{
    std::size_t operator()(const Gear::Skill& skill) const
    {
        using std::hash;
        using std::size_t;
        using std::string;

        // Compute individual hash values for first,
        // second and third and combine them using XOR
        // and bit shifting:

        return hash<string>()(skill.getName()) ^ (hash<int>()(skill.getSkillPoints()) << 1);
    }
};

} // namespace std

#endif // DAUNTLESS_ASS_GEAR_SKILL_HPP