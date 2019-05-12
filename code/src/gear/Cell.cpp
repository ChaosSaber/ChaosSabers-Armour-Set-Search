#include "gear/Cell.hpp"
#include "gear/CellList.hpp"
#include <sstream>

Gear::Cell::Cell(SkillType type) : empty(true), type(type), skill() {}

Gear::Cell::Cell(Skill skill, SkillType type) : skill(std::move(skill)), empty(false), type(type) {}

int Gear::Cell::getSkillPointsFor(const std::string &skillName) const
{
    if (skill.getName() == skillName)
        return skill.getSkillPoints();
    else
        return 0;
}

bool Gear::Cell::isEmpty() const { return empty; }

const std::string &Gear::Cell::getSkillName() const { return skill.getName(); }

const Gear::Skill &Gear::Cell::getSkill() const { return skill; }

std::string Gear::Cell::getCellInfo(const Dictionary &dict) const
{
    if (empty)
    {
        return dict.getTranslationFor(FreeCellSlotToStringKey(type));
    }
    else
    {
        return skill.toString(dict);
    }
}

Gear::SkillType Gear::Cell::getCellType() const { return type; }

bool Gear::Cell::operator==(const Cell &cell) const
{
    if (empty && cell.empty)
        return type == cell.type;
    else
        return skill == cell.skill;
}

Gear::CellList Gear::Cell::operator*(size_t multiplicator) const
{
    CellList cells;
    for (size_t i = 0; i < multiplicator; ++i)
        cells += *this;
    return cells;
}