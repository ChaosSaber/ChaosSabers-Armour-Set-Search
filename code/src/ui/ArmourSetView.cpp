#include "ui/ArmourSetView.hpp"
#include "ui/Translation.hpp"
#include "ui_ArmourSetView.h"
#include <QLabel>
#include <sstream>

QSize Ui::ElidingLabel::sizeHint() const { return QSize(0, QLabel::sizeHint().height()); }

ArmourSetView::ArmourSetView(const Dictionary &dict, const Gear::ArmourSet &set,
                             const Gear::Armoury &armoury, int scrollBarWidth, QWidget *parent)
    : QWidget(parent), ui(new Ui::ArmourSetView), dict(dict), scrollBarWidth(scrollBarWidth)
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
        if (skill.getSkillPoints() > 0)
            addSkill(skill);
    if (uniqueSkills.size() > 0)
    {
        auto label = new QLabel();
        label->setText(getTranslation(dict, "label_unique_effects"));
        ui->verticalLayoutSkills->addWidget(label);
        for (const auto &skill : uniqueSkills)
        {
            auto label = new Ui::ElidingLabel();
            auto content = getTranslation(dict, skill.getName());
            uniqueSkillLabels[label] = content;
            label->setIndent(10);
            label->setToolTip(content);
            setElidedSkill(label, content);
            ui->verticalLayoutSkills->addWidget(label);
        }
    }
    constructing = false;
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

int ArmourSetView::getGearViewWidth() const { return ui->widgetGears->sizeHint().width(); }

void ArmourSetView::setGearViewWidth(int width)
{
    ui->widgetGears->setMaximumWidth(width);
    ui->widgetGears->setMinimumWidth(width);
}

void ArmourSetView::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    for (const auto &skillLabel : uniqueSkillLabels)
        setElidedSkill(skillLabel.first, skillLabel.second);
}

void ArmourSetView::setElidedSkill(QLabel *label, const QString &text)
{
    int skillWidth = ui->widgetSkills->width();
    if (constructing)
        skillWidth = ui->widgetSkills->sizeHint().width();
    QFontMetrics metrics(label->font());
    QString elidedText = metrics.elidedText(
        text, Qt::ElideRight,
        skillWidth - label->indent() - ui->verticalLayoutSkills->spacing() * 2 - scrollBarWidth);
    label->setText(elidedText);
}