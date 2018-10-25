#include "gear/Armour.hpp"
#include <sstream>

Gear::Armour::Armour(ArmourType type, std::string name, std::string description, int tier,
                     unsigned int minDefense, unsigned int maxDefense, Elements elementalResistance,
                     std::vector<Skill> skills, std::vector<std::string> uniqueSkills,
                     SkillType cellType, Rarity rarity)
    : Armour(type, std::move(name), std::move(description), std::vector<int>{tier}, minDefense, maxDefense,
             std::move(elementalResistance), std::move(skills), std::move(uniqueSkills), cellType,
             rarity)
{
}

Gear::Armour::Armour(ArmourType type, std::string name, std::string description,
                     std::vector<int> tiers, unsigned int minDefense, unsigned int maxDefense,
                     Elements elementalResistance, std::vector<Skill> skills,
                     std::vector<std::string> uniqueSkills, SkillType cellType, Rarity rarity)
    : Gear(name, tiers), type(type), description(std::move(description)), minDefense(minDefense),
      maxDefense(maxDefense), elementalResistance(std::move(elementalResistance)),
      skills(std::move(skills)), uniqueSkills(std::move(uniqueSkills)), cell(cellType),
      rarity(rarity)
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
    auto str = dict.getTranslationFor(getName());
    if (getTiers().size() == 1)
    {
        auto tier = getTiers()[0];
        if (tier == 5)
            str += " +5";
        if (tier == 6 && !isExotic())
            str += " +10";
    }
    return str;
}

Gear::SkillList Gear::Armour::getSkills() const { return skills + cell.getSkill(); }

Gear::CellList Gear::Armour::getCells() const { return cell; }

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

const std::vector<std::string> &Gear::Armour::getUniqueSkills() const { return uniqueSkills; }

bool Gear::Armour::isExotic() const { return rarity == Exotic; }
