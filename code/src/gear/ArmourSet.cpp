#include "gear/ArmourSet.hpp"
#include <iostream>
#include <sstream>

Gear::ArmourSet::ArmourSet(Armour head, Armour torso, Armour arms, Armour legs, Weapon weapon,
                           Cell lantern)
    : head(std::move(head)), torso(std::move(torso)), legs(std::move(legs)), arms(std::move(arms)),
      weapon(std::move(weapon)), lantern(std::move(lantern))
{
    gear.push_back(&(this->head));
    gear.push_back(&(this->torso));
    gear.push_back(&(this->legs));
    gear.push_back(&(this->arms));
    gear.push_back(&(this->weapon));
}

Gear::ArmourSet::ArmourSet(const ArmourSet &other)
    : ArmourSet(other.head, other.torso, other.arms, other.legs, other.weapon, other.lantern)
{
}

void Gear::swap(ArmourSet &first, ArmourSet &second)
{
    using std::swap;
    swap(first.head, second.head);
    swap(first.torso, second.torso);
    swap(first.arms, second.arms);
    swap(first.legs, second.legs);
    swap(first.weapon, second.weapon);
    swap(first.lantern, second.lantern);
}

Gear::ArmourSet &Gear::ArmourSet::operator=(ArmourSet other)
{
    swap(*this, other);
    return *this;
}

Gear::ArmourSet::ArmourSet(ArmourSet &&other)
    : ArmourSet(std::move(other.head), std::move(other.torso), std::move(other.arms),
                std::move(other.legs), std::move(other.weapon), std::move(other.lantern))
{
}

bool Gear::ArmourSet::hasFreeCellSlotFor(SkillType type) const
{
    if (lantern.isEmpty() && lantern.getCellType() == type)
        return true;
    for (const auto part : gear)
    {
        if (part->hasFreeCellSlotFor(type))
            return true;
    }
    return false;
}

bool Gear::ArmourSet::addCell(Cell cell)
{
    if (cell.getSkillName() == "medic" && lantern.isEmpty())
    {
        std::string ss;
        ss = "5";
    }
    if (lantern.isEmpty() && lantern.getCellType() == cell.getCellType())
    {
        lantern = std::move(cell);
        return true;
    }
    for (auto part : gear)
    {
        if (part->addCell(cell))
            return true;
    }
    return false;
}

std::string Gear::ArmourSet::getGearInfo(const Dictionary &dict) const
{
    std::stringstream ss;
    ss << weapon.getGearInfo(dict) << std::endl
       << head.getGearInfo(dict) << std::endl
       << torso.getGearInfo(dict) << std::endl
       << arms.getGearInfo(dict) << std::endl
       << legs.getGearInfo(dict) << std::endl
       << getCells().toString(dict) << "Skills:" << std::endl
       << getSkills().toString(dict);
    return ss.str();
}

Gear::SkillList Gear::ArmourSet::getSkills() const
{
    SkillList skills;
    for (const auto part : gear)
        skills += part->getSkills();
    skills += lantern.getSkill();
    return skills;
}

int Gear::ArmourSet::getSkillPointsFor(const std::string &skill) const
{
    int sum = 0;
    sum += lantern.getSkillPointsFor(skill);
    for (const auto part : gear)
        sum += part->getSkillPointsFor(skill);
    return sum;
}

Gear::CellList Gear::ArmourSet::getCells() const
{
    CellList cells = lantern;
    for (const auto part : gear)
    {
        cells += part->getCells();
    }
    return cells;
}

const Gear::Armour &Gear::ArmourSet::getHead() const { return head; }

const Gear::Armour &Gear::ArmourSet::getTorso() const { return torso; }

const Gear::Armour &Gear::ArmourSet::getArms() const { return arms; }

const Gear::Armour &Gear::ArmourSet::getLegs() const { return legs; }

const Gear::Weapon &Gear::ArmourSet::getWeapon() const { return weapon; }

const Gear::Cell &Gear::ArmourSet::getLantern() const { return lantern; }

bool Gear::ArmourSet::hasUniqueSkill() const
{
    for (auto gear : this->gear)
        if (gear->hasUniqueSkill())
            return true;
    return false;
}
