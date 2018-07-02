#include "gear/Weapon.hpp"

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