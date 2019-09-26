#include "gear/SkillList.hpp"
#include "gear/Armoury.hpp"
#include <algorithm>
#include <gear/Armoury.hpp>
#include <sstream>

Gear::SkillList::SkillList(Skill skill) { skills.push_back(skill); }

Gear::SkillList::SkillList(const std::vector<Skill>& skills)
{
    for (const auto& skill : skills)
        *this += skill;
}

bool Gear::skillSorter(const Skill& lhs, const Skill& rhs) { return lhs > rhs; }

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

bool Gear::SkillList::empty() const { return skills.empty(); }

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

const Gear::SkillList& Gear::SkillList::operator-=(const Skill& skill)
{
    if (skill.getId() == Skill::NO_SKILL)
        return *this;
    for (auto& skill_ : skills)
    {
        if (skill_.getId() == skill.getId())
        {
            if (skill_.skillPoints_ >= skill.getSkillPoints())
            {
                skill_.skillPoints_ -= skill.getSkillPoints();
            }
            else
            {
                skill_.skillPoints_ = 0;
            }
            return *this;
        }
    }
    return *this;
}

const Gear::SkillList& Gear::SkillList::operator-=(const SkillList& skillList)
{
    for (const auto& skill : skillList)
        *this -= skill;
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

Gear::WantedSkillList::WantedSkillList(const Armoury& armoury) : WantedSkillList({}, armoury) {}

Gear::WantedSkillList::WantedSkillList(const std::vector<Skill>& wantedSkills,
                                       const Armoury& armoury)
    : armoury_(armoury), wanted_(armoury.getSkillInfos().size(), false),
      levels_(armoury.getSkillInfos().size(), 0), skillLevelPerType_({})
{
    for (auto& skill : wantedSkills)
    {
        if (skill.getSkillPoints() == 0)
            continue;
        ids_.insert(skill.getId());
        wanted_[skill.getId()] = true;
        levels_[skill.getId()] = skill.getSkillPoints();
        skillLevelPerType_[armoury.getSkillTypeFor(skill.getId())] += skill.getSkillPoints();
    }
}

bool Gear::WantedSkillList::isWanted(size_t skillId) const { return wanted_[skillId]; }

size_t Gear::WantedSkillList::getWantedLevel(size_t skillId) const { return levels_[skillId]; }

const std::set<size_t>& Gear::WantedSkillList::getWantedSkills() const { return ids_; }

size_t Gear::WantedSkillList::getSkillLevelForType(SkillType type) const
{
    return skillLevelPerType_[type];
}

const Gear::WantedSkillList& Gear::WantedSkillList::operator+=(const Skill& skill)
{
    if (skill.getId() == 0)
        return *this;
    wanted_[skill.getId()] = true;
    levels_[skill.getId()] += skill.getSkillPoints();
    ids_.insert(skill.getId());
    skillLevelPerType_[armoury_.getSkillTypeFor(skill.getId())] += skill.getSkillPoints();
    return *this;
}

const Gear::WantedSkillList& Gear::WantedSkillList::operator-=(const Skill& skill)
{
    if (skill.getId() == 0)
        return *this;
    if (levels_[skill.getId()] > skill.getSkillPoints())
        levels_[skill.getId()] -= skill.getSkillPoints();
    else
    {
        levels_[skill.getId()] = 0;
        wanted_[skill.getId()] = false;
        ids_.erase(skill.getId());
    }
    skillLevelPerType_[armoury_.getSkillTypeFor(skill.getId())] += skill.getSkillPoints();
    return *this;
}
