#ifndef DAUNTLESS_ASS_GEAR_SKILLLIST_HPP
#define DAUNTLESS_ASS_GEAR_SKILLLIST_HPP

#include "Dictionary.hpp"
#include "gear/Skill.hpp"
#include <vector>

namespace Gear
{
bool skillSorter(const Skill& lhs, const Skill& rhs);

class SkillList
{
  public:
    SkillList() {}
    SkillList(Skill skill);
    SkillList(const std::vector<Skill>& skills);
    std::string toString(const Dictionary& dict);
    void sort();

    /**
     * Checks if it contains the given skill
     * @param skill The skill to check
     * @return Returns if it contains the skill
     */
    bool contains(const Skill& skill) const;

    /**
     * Checks if it contains the given skill or has more skillpoints in it
     * @param skill The skill to check.
     * @return Returns if it contains the given skill or has even more skill points in it
     */
    bool containsOrHasBetter(const Skill& skill) const;

    /**
     * inserts a skill into the list if it not already exists
     * @param skill The Skill to insert
     */
    void insert(const Skill& skill);

    /**
     * Inserts all skills from the given list which it does not yet contain
     * @param skill The list of skills to insert
     */
    void insert(const SkillList& skills);

    /**
     *@return Returns the number of skills in the list
     */
    size_t size() const;

    std::vector<Skill>::iterator begin();
    std::vector<Skill>::const_iterator begin() const;
    std::vector<Skill>::iterator end();
    std::vector<Skill>::const_iterator end() const;

    /**
     * @param pos Th position of the wanted object
     * @return A refrence to the wanted skill
     */
    const Skill& operator[](size_t pos) const;
    const SkillList& operator+=(const Skill& skill);
    const SkillList& operator+=(const SkillList& skillList);
    friend SkillList operator+(SkillList lhs, const Skill& rhs);
    friend SkillList operator+(SkillList lhs, const SkillList& rhs);

  private:
    std::vector<Skill> skills;
};
} // namespace Gear

#endif // !DAUNTLESS_ASS_GEAR_SKILLLIST_HPP