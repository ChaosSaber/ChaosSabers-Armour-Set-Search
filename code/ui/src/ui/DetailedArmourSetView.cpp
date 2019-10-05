#include "ui/DetailedArmourSetView.hpp"
#include "ui/GearView.hpp"
#include "ui/SkillLabel.hpp"
#include "ui_DetailedArmourSetView.h"
#include "util/Spacer.hpp"
#include "util/Translation.hpp"

DetailedArmourSetView::DetailedArmourSetView(const Gear::ArmourSet& set, const Dictionary& dict,
                                             const Gear::Armoury& armoury, QWidget* parent)
    : QWidget(parent), ui(new Ui::DetailedArmourSetView)
{
    ui->setupUi(this);

    ui->gearLayout->addWidget(new GearView(set.getWeapon(), dict, armoury));
    ui->gearLayout->addWidget(new GearView(set.getHead(), dict, armoury));
    ui->gearLayout->addWidget(new GearView(set.getTorso(), dict, armoury));
    ui->gearLayout->addWidget(new GearView(set.getArms(), dict, armoury));
    ui->gearLayout->addWidget(new GearView(set.getLegs(), dict, armoury));
    ui->gearLayout->addWidget(new util::VerticalSpacer);

    auto skills = set.getSkills();
    skills.sort();
    for (const auto& skill : skills)
    {
        auto* label = new SkillLabel(skill, armoury, dict);
        auto font = label->font();
        font.setPointSize(10);
        label->setFont(font);
        ui->perkLayout->addWidget(label);
    }
    for (const auto& uniqueSkill : set.getUniqueSkills())
    {
        auto* label = new QLabel(getTranslation(dict, uniqueSkill));
        label->setWordWrap(true);
        auto font = label->font();
        font.setPointSize(10);
        label->setFont(font);
        ui->perkLayout->addWidget(label);
    }
    ui->perkLayout->addWidget(new util::VerticalSpacer);
}

DetailedArmourSetView::~DetailedArmourSetView() { delete ui; }
