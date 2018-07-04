#include "gear/Armour.hpp"
#include <sstream>

Gear::Armour::Armour(ArmourType type, std::string name, unsigned int defense,
                     Elements elementalResistance, Skill skill1, Skill skill2, SkillType cellType)
    : type(type), name(name), defense(defense), elementalResistance(std::move(elementalResistance)),
      skill1(std::move(skill1)), skill2(std::move(skill2)), cell(cellType)
{
}

unsigned int Gear::Armour::getDefense() const { return defense; }

const Gear::Elements &Gear::Armour::getElementalResistance() const { return elementalResistance; }

bool Gear::Armour::hasSkill(const std::string &skill) const
{
    return skill1.getName() == skill || skill2.getName() == skill;
}

int Gear::Armour::getSkillPointsFor(const std::string &skill) const
{
    if (skill1.getName() == skill)
        return skill1.getSkillPoints();
    else if (skill2.getName() == skill)
        return skill2.getSkillPoints();
    else
        return 0;
}

bool Gear::Armour::addCell(Cell cell)
{
    if (hasFreeCellSlotFor(cell.getCellType()))
    {
        this->cell = std::move(cell);
        return true;
    }
    else
        return false;
}

const Gear::Cell &Gear::Armour::getCell() const { return cell; }

bool Gear::Armour::hasFreeCellSlotFor(SkillType type) const
{
    return cell.isEmpty() && cell.getCellType() == type;
}

std::string Gear::Armour::getGearInfo(const Dictionary &dict) const
{
    return dict.getTranslationFor(name);
}

Gear::SkillList Gear::Armour::getSkills() const { return skill1 + skill2 + cell.getSkill(); }

Gear::CellList Gear::Armour::getCells() const { return cell; }

const std::string &Gear::Armour::getName() const { return name; }

std::string Gear::Armour::getToolTip(const Dictionary &dict) const
{
    std::stringstream ss;
    ss << skill1.toString(dict) << std::endl
       << skill2.toString(dict) << std::endl
       << dict.getTranslationFor(cellSlotToStringKey(cell.getCellType()));
    return ss.str();
}
