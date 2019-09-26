#ifndef DAUNTLESS_ASS_GEAR_SKILLLIST_HPP
#define DAUNTLESS_ASS_GEAR_SKILLLIST_HPP

#include "Dictionary.hpp"
#include "gear/Skill.hpp"
#include "gear/SkillType.hpp"
#include <array>
#include <vector>
#include <set>

namespace Gear
{
bool skillSorter(const Skill& lhs, const Skill& rhs);

class SkillList
{
  public:
    SkillList() {}
    SkillList(Skill skill);
    SkillList(const std::vector<Skill>& skills);
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

    /**
     * checks if the skill list is empty
     * @return Returns true  if the list is empty, otherwise false
     */
    bool empty() const;

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
    const SkillList& operator-=(const Skill& skill);
    const SkillList& operator-=(const SkillList& skillList);
    friend SkillList operator+(SkillList lhs, const Skill& rhs);
    friend SkillList operator+(SkillList lhs, const SkillList& rhs);

  private:
    std::vector<Skill> skills;
};

/**
 * Skill list optimized for accessing specific elements. Used for the armour search.
 */
class WantedSkillList
{
  public:
    /**
     * Creates a new wanted skil list.
     * @param armoury An reference to the armoury. Used for initializing the vector size.
     */
    WantedSkillList(const Armoury& armoury);
    /**
     * Creates a new wanted skil list.
     * @param wantedSkills A list of skills which are wanted.
     * @param armoury An reference to the armoury. Used for initializing the vector size.
     */
    WantedSkillList(const std::vector<Skill>& wantedSkills, const Armoury& armoury);

    /**
     * Checks if the given skill ID is a wanted skill.
     * @param skillId The skill ID which needs to be checked.
     * @return Returns true if the skill ID is a wanted skill, otherwise false.
     */
    bool isWanted(size_t skillId) const;

    /**
     * Gets the wanted skill level of the given skill ID.
     * @param skillId The skill ID the level is needed for.
     * @return Returns the wanted skill level.
     */
    size_t getWantedLevel(size_t skillId) const;

    /**
     * Gets all wanted skills.
     * @return Returns a list with the skill IDs.
     */
    const std::set<size_t>& getWantedSkills() const;

    /**
     * gets the overall needed skill level for the specified type
     * @param type The type for which the skill level are needed
     * @return Returns the needed skill level for that type
     */
    size_t getSkillLevelForType(SkillType type) const;

    const WantedSkillList& operator+=(const Skill& skill);
    const WantedSkillList& operator-=(const Skill& skill);

  private:
    const Armoury& armoury_;
    std::vector<bool> wanted_;
    std::vector<size_t> levels_;
    std::set<size_t> ids_;
    std::array<size_t, SkillType::MaxSkillType> skillLevelPerType_;
};
} // namespace Gear

#endif // !DAUNTLESS_ASS_GEAR_SKILLLIST_HPP