#ifndef DAUNTLESS_ASS_GEAR_SKILL_HPP
#define DAUNTLESS_ASS_GEAR_SKILL_HPP

#include "Dictionary.hpp"
#include <functional>

namespace Gear
{
class Armoury;
class Skill
{
  public:
    Skill();
    Skill(size_t id, size_t skillPoints);

    size_t getId() const;
    size_t getSkillPoints() const;
    /**
     * transforms the skill to an human readable string
     * @param dict A dictionary with all necessary translations
     * @param armoury The armoury with all the necessary skill data
     * @return Returns a human readable string
     */
    std::string toString(const Dictionary& dict, const Armoury& armoury) const;
    friend class SkillList;

    bool operator==(const Skill& other) const;
    bool operator<(const Skill& other) const;
    bool operator>(const Skill& other) const;

    static constexpr size_t NO_SKILL = 0;
  private:
    size_t id_;
    size_t skillPoints_;
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

        // Compute individual hash values for first,
        // second and third and combine them using XOR
        // and bit shifting:

        return hash<size_t>()(skill.getId()) ^ (hash<size_t>()(skill.getSkillPoints()) << 1);
    }
};

} // namespace std

#endif // DAUNTLESS_ASS_GEAR_SKILL_HPP