#include "gear/Gear.hpp"
#include <sstream>

Gear::Gear::Gear(std::shared_ptr<GearInfo>& info, int level, const Skill& skill,
                 std::shared_ptr<std::vector<std::string>>& uniqueSkills,
                 const std::vector<Cell>& cells)
    : info_(info), level_(level), skill_(skill), uniqueSkills_(uniqueSkills), cells_(cells)
{
}

Gear::Gear::Gear(std::shared_ptr<GearInfo>&& info, int level, Skill&& skill,
                 std::shared_ptr<std::vector<std::string>>&& uniqueSkills,
                 std::vector<Cell>&& cells)
    : info_(std::move(info)), level_(level), skill_(std::move(skill)),
      uniqueSkills_(std::move(uniqueSkills)), cells_(std::move(cells))
{
}

const Gear::Gear& Gear::Gear::operator=(const Gear& other)
{
    info_ = other.info_;
    level_ = level_;
    skill_ = other.skill_;
    uniqueSkills_ = other.uniqueSkills_;
    cells_ = other.cells_;
    return *this;
}

const Gear::Gear& Gear::Gear::operator=(Gear&& other)
{
    info_ = std::move(other.info_);
    level_ = std::move(level_);
    skill_ = std::move(other.skill_);
    uniqueSkills_ = std::move(other.uniqueSkills_);
    cells_ = std::move(other.cells_);
    return *this;
}

const std::string& Gear::Gear::getName() const { return info_->name_; }

int Gear::Gear::getLevel() const { return level_; }

const std::string& Gear::Gear::getDescription() const { return info_->description_; }

bool Gear::Gear::hasUniqueSkill() const
{
    return uniqueSkills_ != nullptr && uniqueSkills_->size() != 0;
}

const std::vector<std::string>& Gear::Gear::getUniqueSkills() const { return *uniqueSkills_; }

const Gear::Skill& Gear::Gear::getInnateSkills() const { return skill_; }

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
    size_t sum = 0;
    if (skill_.getId() > 0 && skill_.getId() == skillId)
        sum += skill_.getSkillPoints();
    for (const auto& cell : cells_)
    {
        if (!cell.isEmpty() && cell.getSkillId() == skillId)
            sum += cell.getSkill().getSkillPoints();
    }
    return sum;
}

std::string Gear::Gear::getGearInfo(const Dictionary& dict) const
{
    return dict.getTranslationFor(info_->name_) + " +" + std::to_string(level_);
}

const Gear::Elements& Gear::Gear::getElements() const { return info_->elements_; }

std::string Gear::Gear::getToolTip(const Dictionary& dict, const Armoury& armoury) const
{
    std::stringstream ss;
    ss << info_->description_ << std::endl;
    if (skill_.getSkillPoints() > 0)
        ss << skill_.toString(dict, armoury) << std::endl;
    if (hasUniqueSkill())
    {
        for (const auto& skill : (*uniqueSkills_))
        {
            ss << skill << std::endl;
        }
    }
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

void Gear::Gear::removeAllCells()
{
    for (auto& cell : cells_)
    {
        if (!cell.isEmpty())
        {
            cell.clear();
        }
    }
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

Gear::GearInfo::GearInfo(const std::string& name, const std::string& description,
                         const Elements& elements)
    : name_(name), description_(description), elements_(elements)
{
}

Gear::GearInfo::GearInfo(std::string&& name, std::string&& description, Elements&& elements)
    : name_(std::move(name)), description_(std::move(description)), elements_(std::move(elements))
{
}
