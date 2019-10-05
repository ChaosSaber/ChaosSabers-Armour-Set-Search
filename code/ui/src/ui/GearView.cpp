#include "ui/GearView.hpp"
#include "ui/CellDisplay.hpp"
#include "ui/ElementalLabel.hpp"
#include "ui/SkillLabel.hpp"
#include "ui_GearView.h"
#include "util/Spacer.hpp"
#include "util/Translation.hpp"

GearView::GearView(const Gear::Gear& gear, const Dictionary& dict, const Gear::Armoury& armoury,
                   QWidget* parent)
    : QWidget(parent), ui(new Ui::GearView)
{
    ui->setupUi(this);
    ui->gearName->setText(QString::fromStdString(gear.getGearInfo(dict)));
    if (gear.getElementalStrength())
        ui->powerElementalRating->addWidget(
            new ElementalLabel(*gear.getElementalStrength(), true, dict));
    if (gear.getElementalWeakness())
        ui->powerElementalRating->addWidget(
            new ElementalLabel(*gear.getElementalWeakness(), false, dict));
    ui->powerElementalRating->addWidget(new util::HorizontalSpacer);
    if (gear.getInnateSkills().getId() != 0) {
        auto* label = new SkillLabel(gear.getInnateSkills(), armoury, dict);
        auto font = label->font();
        font.setPointSize(10);
        label->setFont(font);
        ui->gearDescriptionLayout->addWidget(label);
    }
    if (gear.hasUniqueSkill())
    {
        for (const auto& uniqueSkill : gear.getUniqueSkills())
        {
            auto* label = new QLabel(getTranslation(dict, uniqueSkill));
            label->setWordWrap(true);
            auto font = label->font();
            font.setPointSize(10);
            label->setFont(font);
            ui->gearDescriptionLayout->addWidget(label);
        }
    }
    ui->gearDescriptionLayout->addWidget(new util::VerticalSpacer);
    // ui->horizontalLayout->addWidget(new util::HorizontalSpacer);
    for (const auto& [cell, count] : gear.getCellList())
    {
        if (cell.isEmpty())
            continue;
        for (size_t i = 0; i < count; ++i)
        {
            ui->horizontalLayout->addWidget(new CellDisplay(cell, dict, armoury));
        }
    }
}

GearView::~GearView() { delete ui; }
