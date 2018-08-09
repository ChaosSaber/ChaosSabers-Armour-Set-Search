#include "gear/Weapon.hpp"
#include <sstream>

Gear::Weapon::Weapon(WeaponType type, std::string name, std::string description, int tier,
                     unsigned int minDamage, unsigned int maxDamage, Elements elementalDamage,
                     std::vector<Skill> skills, std::vector<std::string> uniqueSkills,
                     SkillType cellType1, SkillType cellType2, Rarity rarity)
    : type(type), name(std::move(name)), description(std::move(description)), minDamage(minDamage),
      maxDamage(maxDamage), elementalDamage(std::move(elementalDamage)), skills(std::move(skills)),
      uniqueSkills(std::move(uniqueSkills)), cell1(cellType1), cell2(cellType2), rarity(rarity), tier(tier)
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
    auto str = dict.getTranslationFor(name);
    if (tier == 5)
        str += " +5";
    if (tier == 6 && !isExotic())
        str += " +10";
    return str;
}

Gear::SkillList Gear::Weapon::getSkills() const
{
    return skills + cell1.getSkill() + cell2.getSkill();
}

int Gear::Weapon::getSkillPointsFor(const std::string &skill) const
{
    for (const auto &skill_ : skills)
        if (skill_.getName() == skill)
            return skill_.getSkillPoints();
    return 0;
}

Gear::CellList Gear::Weapon::getCells() const { return cell1 + cell2; }

const std::string &Gear::Weapon::getName() const { return name; }

std::string Gear::Weapon::getToolTip(const Dictionary &dict) const
{
    std::stringstream ss;
    ss << description << std::endl;
    for (const auto &skill : skills)
        ss << skill.toString(dict) << std::endl;
    for (const auto &skill : uniqueSkills)
        ss << skill << std::endl;
    if (cell1.getCellType() == cell2.getCellType())
        ss << "2 " << dict.getTranslationFor(cellSlotToStringKey(cell1.getCellType()));
    else
        ss << dict.getTranslationFor(cellSlotToStringKey(cell1.getCellType())) << std::endl
           << dict.getTranslationFor(cellSlotToStringKey(cell2.getCellType()));
    return ss.str();
}

const Gear::Cell &Gear::Weapon::getCell1() const { return cell1; }
const Gear::Cell &Gear::Weapon::getCell2() const { return cell2; }

bool Gear::Weapon::hasUniqueSkill() const { return !uniqueSkills.empty(); }

std::vector<std::string> Gear::Weapon::getUniqueSkills() const { return uniqueSkills; }

bool Gear::Weapon::isExotic() const { return rarity == Exotic; }

int Gear::Weapon::getTier() const { return tier; }
