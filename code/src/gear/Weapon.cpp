#include "gear/Weapon.hpp"
#include <sstream>

Gear::Weapon::Weapon(WeaponType type, std::string name, unsigned int damage,
                     Elements elementalDamage, Skill skill1, Skill skill2, SkillType cellType1,
                     SkillType cellType2)
    : type(type), name(std::move(name)), damage(damage),
      elementalDamage(std::move(elementalDamage)), skill1(std::move(skill1)),
      skill2(std::move(skill2)), cell1(cellType1), cell2(cellType2)
{
}

bool Gear::Weapon::hasFreeCellSlotFor(SkillType type) const
{
    return cell1.isEmpty() && cell1.getCellType() == type ||
           cell2.isEmpty() && cell2.getCellType() == type;
}

bool Gear::Weapon::addCell(Cell cell)
{
    if (cell1.isEmpty() && cell1.getCellType() == cell.getCellType())
    {
        cell1 = std::move(cell);
        return true;
    }
    if (cell2.isEmpty() && cell2.getCellType() == cell.getCellType())
    {
        cell2 = std::move(cell);
        return true;
    }
    return false;
}

std::string Gear::Weapon::getGearInfo(const Dictionary &dict) const
{
    return dict.getTranslationFor(name);
}

Gear::SkillList Gear::Weapon::getSkills() const
{
    return skill1 + skill2 + cell1.getSkill() + cell2.getSkill();
}

int Gear::Weapon::getSkillPointsFor(const std::string &skill) const
{
    if (skill1.getName() == skill)
        return skill1.getSkillPoints();
    else if (skill2.getName() == skill)
        return skill2.getSkillPoints();
    else
        return 0;
}

Gear::CellList Gear::Weapon::getCells() const { return cell1 + cell2; }

const std::string &Gear::Weapon::getName() const { return name; }

std::string Gear::Weapon::getToolTip(const Dictionary &dict) const
{
    std::stringstream ss;
    if (skill1.getName() != "")
        ss << skill1.toString(dict) << std::endl;
    if (skill2.getName() != "")
        ss << skill2.toString(dict) << std::endl;
    if (cell1.getCellType() == cell2.getCellType())
        ss << "2 " << dict.getTranslationFor(cellSlotToStringKey(cell1.getCellType()));
    else
        ss << dict.getTranslationFor(cellSlotToStringKey(cell1.getCellType())) << std::endl
           << dict.getTranslationFor(cellSlotToStringKey(cell2.getCellType()));
    return ss.str();
}

const Gear::Cell &Gear::Weapon::getCell1() const { return cell1; }
const Gear::Cell &Gear::Weapon::getCell2() const { return cell2; }

bool Gear::Weapon::hasUniqueSkill() const { return skill1.isUnique() || skill2.isUnique(); }