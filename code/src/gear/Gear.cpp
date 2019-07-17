#include "gear/Gear.hpp"
#include <sstream>

Gear::Gear::Gear(const std::string& name, const std::string& description, int level,
                 const Skill& skill, const std::vector<std::string>& uniqueSkills,
                 const std::vector<Cell>& cells, const Elements& elements)
    : name_(name), description_(description), level_(level), skill_(skill),
      uniqueSkills_(uniqueSkills), cells_(cells), elements_(elements)
{
}

Gear::Gear::Gear(std::string&& name, std::string&& description, int level, Skill&& skill,
                 std::vector<std::string>&& uniqueSkills, std::vector<Cell>&& cells,
                 const Elements&& elements)
    : name_(std::move(name)), description_(std::move(description)), level_(level),
      skill_(std::move(skill)), uniqueSkills_(std::move(uniqueSkills)), cells_(std::move(cells)),
      elements_(std::move(elements))
{
}

const Gear::Gear& Gear::Gear::operator=(Gear&& other)
{
    name_ = std::move(other.name_);
    description_ = std::move(other.description_);
    level_ = std::move(level_);
    skill_ = std::move(other.skill_);
    uniqueSkills_ = std::move(other.uniqueSkills_);
    cells_ = std::move(other.cells_);
    elements_ = std::move(other.elements_);
    return *this;
}

const std::string& Gear::Gear::getName() const { return name_; }

int Gear::Gear::getLevel() const { return level_; }

const std::string& Gear::Gear::getDescription() const { return description_; }

bool Gear::Gear::hasUniqueSkill() const { return uniqueSkills_.size() != 0; }

const std::vector<std::string>& Gear::Gear::getUniqueSkills() const { return uniqueSkills_; }

Gear::SkillList Gear::Gear::getSkills() const
{
    SkillList skills;
    if (skill_.getSkillPoints() > 0)
        skills += skill_;
    for (auto& cell : cells_)
        if (!cell.isEmpty())
            skills += cell.getSkill();
    return skills;
}

bool Gear::Gear::hasSkill(size_t skillId) const
{
    if (skill_.getId() == 0)
        return false;
    return skill_.getId() == skillId;
}

size_t Gear::Gear::getSkillPointsFor(size_t skillId) const
{
    if (skill_.getId()>0 && skill_.getId() == skillId)
        return skill_.getSkillPoints();
    return 0;
}

std::string Gear::Gear::getGearInfo(const Dictionary& dict) const
{
    return dict.getTranslationFor(name_) + " +" + std::to_string(level_);
}

const Gear::Elements& Gear::Gear::getElements() const { return elements_; }

std::string Gear::Gear::getToolTip(const Dictionary& dict, const Armoury& armoury) const
{
    std::stringstream ss;
    ss << description_ << std::endl;
    if (skill_.getSkillPoints() > 0)
        ss << skill_.toString(dict, armoury) << std::endl;
    for (const auto& skill : uniqueSkills_)
        ss << skill << std::endl;
    for (auto& cell : cells_)
        if (cell.getCellType() != SkillType::None)
            ss << dict.getTranslationFor(cellSlotToStringKey(cell.getCellType())) << " ";
    return ss.str();
}

bool Gear::Gear::hasFreeCellSlotFor(SkillType type) const
{
    for (auto& cell : cells_)
        if (cell.isEmpty() && cell.getCellType() == type)
            return true;
    return false;
}

Gear::CellList Gear::Gear::getCellList() const { return cells_; }

bool Gear::Gear::addCell(const Cell& cell)
{
    for (auto& cell_ : cells_)
    {
        if (cell_.isEmpty() && cell_.getCellType() == cell.getCellType())
        {
            cell_ = cell;
            return true;
        }
    }
    return false;
}
