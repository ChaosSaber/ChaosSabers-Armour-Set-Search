#include "gear/SkillList.hpp"
#include <algorithm>
#include <sstream>

Gear::SkillList::SkillList(Skill skill) { skills.push_back(skill); }

Gear::SkillList::SkillList(const std::vector<Skill>& skills)
{
    for (const auto& skill : skills)
        *this += skill;
}

bool Gear::skillSorter(const Skill& lhs, const Skill& rhs)
{
    return lhs > rhs;
}

void Gear::SkillList::sort() { std::sort(skills.begin(), skills.end(), skillSorter); }

bool Gear::SkillList::contains(const Skill& skill) const
{
    return std::find(skills.begin(), skills.end(), skill) != skills.end();
}

bool Gear::SkillList::containsOrHasBetter(const Skill& skill) const
{
    for (auto& skill_ : skills)
    {
        if (skill_.getId() == skill.getId())
        {
            return skill_.skillPoints_ >= skill.skillPoints_;
        }
    }
    return false;
}

void Gear::SkillList::insert(const Skill& skill)
{
    if (contains(skill))
        return;
    skills.push_back(skill);
}

void Gear::SkillList::insert(const SkillList& skills)
{
    for (auto& skill : skills)
        insert(skill);
}

size_t Gear::SkillList::size() const { return skills.size(); }

std::vector<Gear::Skill>::iterator Gear::SkillList::begin() { return skills.begin(); }

std::vector<Gear::Skill>::const_iterator Gear::SkillList::begin() const { return skills.cbegin(); }

std::vector<Gear::Skill>::iterator Gear::SkillList::end() { return skills.end(); }

std::vector<Gear::Skill>::const_iterator Gear::SkillList::end() const { return skills.cend(); }

const Gear::Skill& Gear::SkillList::operator[](size_t pos) const { return skills[pos]; }

const Gear::SkillList& Gear::SkillList::operator+=(const Skill& skill)
{
    if (skill.getId() == Skill::NO_SKILL)
        return *this;
    for (auto& skill_ : skills)
    {
        if (skill_.getId() == skill.getId())
        {
            skill_.skillPoints_ += skill.getSkillPoints();
            return *this;
        }
    }
    skills.push_back(skill);
    return *this;
}

const Gear::SkillList& Gear::SkillList::operator+=(const SkillList& skillList)
{
    for (const auto& skill : skillList)
        *this += skill;
    return *this;
}

Gear::SkillList Gear::operator+(SkillList lhs, const Skill& rhs)
{
    lhs += rhs;
    return lhs;
}

Gear::SkillList Gear::operator+(SkillList lhs, const SkillList& rhs)
{
    lhs += rhs;
    return lhs;
}