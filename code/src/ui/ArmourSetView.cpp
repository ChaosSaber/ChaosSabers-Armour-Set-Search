#include "ui/ArmourSetView.hpp"
#include "ui/Translation.hpp"
#include "ui_ArmourSetView.h"
#include <QLabel>
#include <sstream>

ArmourSetView::ArmourSetView(const Dictionary &dict, const Gear::ArmourSet &set,
                             const Gear::Armoury &armoury, QWidget *parent)
    : QWidget(parent), ui(new Ui::ArmourSetView), dict(dict)
{
    ui->setupUi(this);

    addGear(set.getWeapon());
    addGear(set.getHead());
    addGear(set.getTorso());
    addGear(set.getArms());
    addGear(set.getLegs());

    auto cells = set.getCells();
    cells.sort();
    for (const auto &cell : cells)
        addCell(cell);

    auto skills = set.getSkills();
    skills.sort();
    std::vector<Gear::Skill> uniqueSkills;
    for (const auto &skill : skills)
    {
        if (armoury.getSkillTypeFor(skill.getName()) == Gear::SkillType::Unique)
        {
            uniqueSkills.push_back(skill);
        }
        else if (skill.getSkillPoints() > 0)
            addSkill(skill);
    }
    if (uniqueSkills.size() > 0)
    {
        auto label = new QLabel();
        label->setText(getTranslation(dict, "label_unique_effects"));
        ui->verticalLayoutSkills->addWidget(label);
        for (const auto &skill : uniqueSkills)
        {
            auto label = new QLabel();
            auto content = getTranslation(dict, skill.getName());
            QFontMetrics metrics(label->font());
            QString elidedText = metrics.elidedText(content, Qt::ElideRight, width() * 0.4); // currently the armoursetview is divided 50/50 into skills and gear/cells
            label->setText(elidedText);
            //label->setWordWrap(true);
            label->setIndent(10);
            label->setToolTip(content);
            ui->verticalLayoutSkills->addWidget(label);
        }
    }
}

ArmourSetView::~ArmourSetView() { delete ui; }

void ArmourSetView::addGear(const Gear::Gear &gear)
{
    QLabel *label = new QLabel();
    label->setText(QString::fromStdString(gear.getGearInfo(dict)));
    label->setToolTip(QString::fromStdString(gear.getToolTip(dict)));
    ui->verticalLayoutGearParts->addWidget(label);
}

void ArmourSetView::addCell(const std::pair<Gear::Cell, int> &cell)
{
    QLabel *label = new QLabel();
    std::stringstream ss;
    ss << cell.second << "x " << cell.first.getCellInfo(dict);
    label->setText(QString::fromStdString(ss.str()));
    ui->verticalLayoutGearParts->addWidget(label);
}

void ArmourSetView::addSkill(const Gear::Skill &skill)
{
    QLabel *label = new QLabel();
    label->setText(QString::fromStdString(skill.toString(dict)));
    ui->verticalLayoutSkills->addWidget(label);
}