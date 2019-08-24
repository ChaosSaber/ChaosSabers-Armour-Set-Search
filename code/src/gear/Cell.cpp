#include "gear/Cell.hpp"
#include "gear/CellList.hpp"
#include <sstream>

Gear::Cell::Cell(SkillType type) : empty(true), type(type), skill() {}

Gear::Cell::Cell(Skill skill, SkillType type) : skill(std::move(skill)), empty(false), type(type) {}

size_t Gear::Cell::getSkillPointsFor(size_t skillId) const
{
    if (skill.getId() == skillId)
        return skill.getSkillPoints();
    else
        return 0;
}

size_t Gear::Cell::getSkillId() const { return skill.getId(); }

bool Gear::Cell::isEmpty() const { return empty; }

const Gear::Skill &Gear::Cell::getSkill() const { return skill; }

std::string Gear::Cell::getCellInfo(const Dictionary &dict, const Armoury& armoury) const
{
    if (empty)
    {
        return dict.getTranslationFor(FreeCellSlotToStringKey(type));
    }
    else
    {
        return skill.toString(dict, armoury);
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
    cells += {*this, multiplicator};
    return cells;
}