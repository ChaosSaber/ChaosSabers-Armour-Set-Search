#ifndef DAUNTLESS_ASS_GEAR_SKILLLIST_HPP
#define DAUNTLESS_ASS_GEAR_SKILLLIST_HPP

#include "Dictionary.hpp"
#include "gear/Skill.hpp"
#include <vector>

namespace Gear
{
bool skillSorter(const Skill &lhs, const Skill &rhs);

class SkillList
{
  public:
    SkillList() {}
    SkillList(Skill skill);
    std::string toString(const Dictionary &dict);
    void sort();

    std::vector<Skill>::iterator begin();
    std::vector<Skill>::const_iterator begin() const;
    std::vector<Skill>::iterator end();
    std::vector<Skill>::const_iterator end() const;
    const SkillList &operator+=(const Skill &skill);
    const SkillList &operator+=(const SkillList &skillList);
    friend SkillList operator+(SkillList lhs, const Skill &rhs);
    friend SkillList operator+(SkillList lhs, const SkillList &rhs);

  private:
    std::vector<Skill> skills;
};
} // namespace Gear

#endif // !DAUNTLESS_ASS_GEAR_SKILLLIST_HPP