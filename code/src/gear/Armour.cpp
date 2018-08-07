#include "gear/Armour.hpp"
#include <sstream>

Gear::Armour::Armour(ArmourType type, std::string name, std::string description, int tier,
                     unsigned int minDefense, unsigned int maxDefense, Elements elementalResistance,
                     std::vector<Skill> skills, std::vector<std::string> uniqueSkills,
                     SkillType cellType, Rarity rarity)
    : type(type), name(std::move(name)), tier(tier), description(std::move(description)),
      minDefense(minDefense), maxDefense(maxDefense),
      elementalResistance(std::move(elementalResistance)), skills(std::move(skills)),
      uniqueSkills(std::move(uniqueSkills)), cell(cellType), rarity(rarity)
{
}

unsigned int Gear::Armour::getMinDefense() const { return minDefense; }

unsigned int Gear::Armour::getMaxDefense() const { return maxDefense; }

const Gear::Elements &Gear::Armour::getElementalResistance() const { return elementalResistance; }

bool Gear::Armour::hasSkill(const std::string &skill) const
{
    for (const auto &skill_ : skills)
        if (skill_.getName() == skill)
            return true;
    return false;
}

int Gear::Armour::getSkillPointsFor(const std::string &skill) const
{
    for (const auto &skill_ : skills)
        if (skill_.getName() == skill)
            return skill_.getSkillPoints();
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

Gear::SkillList Gear::Armour::getSkills() const { return skills + cell.getSkill(); }

Gear::CellList Gear::Armour::getCells() const { return cell; }

const std::string &Gear::Armour::getName() const { return name; }

std::string Gear::Armour::getToolTip(const Dictionary &dict) const
{
    std::stringstream ss;
    ss << description << std::endl;
    for (const auto &skill : skills)
        ss << skill.toString(dict) << std::endl;
    for (const auto &skill : uniqueSkills)
        ss << skill << std::endl;
    ss << dict.getTranslationFor(cellSlotToStringKey(cell.getCellType()));
    return ss.str();
}

bool Gear::Armour::hasUniqueSkill() const { return uniqueSkills.size() != 0; }

std::vector<std::string> Gear::Armour::getUniqueSkills() const { return uniqueSkills; }

bool Gear::Armour::isExotic() const { return rarity == Exotic; }
