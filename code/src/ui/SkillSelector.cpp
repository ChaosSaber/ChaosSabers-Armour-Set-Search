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
}

void SkillSelector::getSkill(std::vector<Gear::Skill> &skills)
{
    if (skillname->currentIndex() == 0 || skillValue->value() == 0)
        return;
    Gear::Skill skill(
        this->skills[(Gear::SkillType)skillType->currentIndex()][skillname->currentIndex() - 1]
            ->getName(),
        skillValue->value());
    skills.push_back(skill);
}