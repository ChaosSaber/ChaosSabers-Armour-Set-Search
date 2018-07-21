#include "ui/SkillSelector.hpp"
#include "ui/Translation.hpp"
#include <QLineEdit>

SkillSelector::SkillSelector(
    const Dictionary &dict,
    std::unordered_map<Gear::SkillType, std::vector<const Gear::SkillInfo *>> &skills,
    QGridLayout *layout)
    : skills(skills), dict(dict)
{
    skillType = new QComboBox();
    skillname = new QComboBox();
    skillValue = new QSpinBox();
    skillValue->setMinimum(0);
    skillValue->setMaximum(6);
    auto row = layout->rowCount();
    layout->addWidget(skillType, row, 0);
    layout->addWidget(skillname, row, 1);
    layout->addWidget(skillValue, row, 2);

    connect(skillType, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this](int index) { filterChanged((Gear::SkillType)index); });
    setSkillTypeFilter();
}

void SkillSelector::setSkillTypeFilter()
{
    skillType->addItems(
        QStringList() << getTranslation(dict, "all")
                      << getTranslation(dict, Gear::SkillTypeToStringKey(Gear::SkillType::Power))
                      << getTranslation(dict,
                                        Gear::SkillTypeToStringKey(Gear::SkillType::Technique))
                      << getTranslation(dict, Gear::SkillTypeToStringKey(Gear::SkillType::Mobility))
                      << getTranslation(dict, Gear::SkillTypeToStringKey(Gear::SkillType::Defense))
                      << getTranslation(dict,
                                        Gear::SkillTypeToStringKey(Gear::SkillType::Utility)));
    skillType->setCurrentIndex(0);
}

void SkillSelector::filterChanged(Gear::SkillType filter)
{
    skillname->clear();
    skillname->addItem("");
    for (auto skill : skills[filter])
        skillname->addItem(getTranslation(dict, skill->getName()));
    skillname->setCurrentIndex(0);
}

void SkillSelector::getSkill(std::vector<Gear::Skill> &skills) const
{
    if (skillname->currentIndex() == 0 || skillValue->value() == 0)
        return;
    Gear::Skill skill(getSkillName(), skillValue->value());
    skills.push_back(skill);
}

std::string SkillSelector::getSkillName() const
{
    if (skillname->currentIndex() == 0)
        return "";
    return skills[(Gear::SkillType)skillType->currentIndex()][skillname->currentIndex() - 1]
        ->getName();
}

void SkillSelector::set(const Options::SkillSearch &search)
{
    skillType->setCurrentIndex(search.filter);
    skillValue->setValue(search.skillLevel);
    if (search.skillName.empty())
        skillname->setCurrentIndex(0);
    else
    {
        for (int i = 0; i < skills[search.filter].size(); ++i)
        {
            if (search.skillName == skills[search.filter][i]->getName())
                skillname->setCurrentIndex(i + 1);
        }
    }
}

Options::SkillSearch SkillSelector::getSearchSettings() const
{
    Options::SkillSearch search;
    search.filter = (Gear::SkillType)skillType->currentIndex();
    search.skillName = getSkillName();
    search.skillLevel = skillValue->value();
    return search;
}

void SkillSelector::clear()
{
    skillType->setCurrentIndex(0);
    skillValue->setValue(0);
    skillname->setCurrentIndex(0);
}