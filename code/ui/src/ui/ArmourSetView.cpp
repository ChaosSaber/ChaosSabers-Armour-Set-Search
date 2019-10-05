#include "ui/ArmourSetView.hpp"
#include "ui/SkillLabel.hpp"
#include "ui_ArmourSetView.h"
#include "util/Export.hpp"
#include "util/Translation.hpp"
#include <QClipboard>
#include <QFileDialog>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <fstream>
#include <sstream>

QSize Ui::ElidingLabel::sizeHint() const { return QSize(0, QLabel::sizeHint().height()); }

ArmourSetView::ArmourSetView(const Dictionary& dict, Options& options, const Gear::ArmourSet& set,
                             const Gear::Armoury& armoury, int scrollBarWidth,
                             Gear::SkillList& wantedSkills, QWidget* parent)
    : QWidget(parent), ui(new Ui::ArmourSetView), dict(dict), options(options),
      scrollBarWidth(scrollBarWidth), armourSet(set), armoury(armoury)
{
    ui->setupUi(this);

    addGear(set.getWeapon());
    addGear(set.getHead());
    addGear(set.getTorso());
    addGear(set.getArms());
    addGear(set.getLegs());

    auto cells = set.getCellList();
    cells.sort();
    for (const auto& cell : cells)
        if (cell.first.getCellType() != Gear::SkillType::None)
            addCell(cell);

    auto skills = set.getAdditionalSkills(wantedSkills);
    skills.sort();
    for (const auto& skill : skills)
        if (skill.getSkillPoints() > 0)
            addSkill(skill);
    const auto& uniqueEffects = set.getUniqueSkills();
    if (uniqueEffects.size() > 0)
    {
        auto label = new QLabel();
        label->setText(getTranslation(dict, "label_unique_effects"));
        ui->verticalLayoutSkills->addWidget(label);
        for (const auto& effect : uniqueEffects)
        {
            auto label = new Ui::ElidingLabel();
            auto content = getTranslation(dict, effect);
            uniqueSkillLabels[label] = content;
            label->setIndent(10);
            label->setToolTip(content);
            setElidedSkill(label, content);
            ui->verticalLayoutSkills->addWidget(label);
        }
    }
    constructing = false;

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QWidget::customContextMenuRequested,
            [this](const QPoint& pos) { showContextMenu(pos); });
}

ArmourSetView::~ArmourSetView() { delete ui; }

void ArmourSetView::addGear(const Gear::Gear& gear)
{
    QLabel* label = new QLabel();
    label->setText(QString::fromStdString(gear.getGearInfo(dict)));
    label->setToolTip(QString::fromStdString(gear.getToolTip(dict, armoury)));
    ui->verticalLayoutGearParts->addWidget(label);
}

void ArmourSetView::addCell(const std::pair<Gear::Cell, int>& cell)
{
    QLabel* label = new QLabel();
    std::stringstream ss;
    ss << cell.second << "x " << cell.first.getCellInfo(dict, armoury);
    label->setText(QString::fromStdString(ss.str()));
    ui->verticalLayoutGearCells->addWidget(label);
}

void ArmourSetView::addSkill(const Gear::Skill& skill)
{
    ui->verticalLayoutSkills->addWidget(new SkillLabel(skill, armoury, dict));
}

int ArmourSetView::getGearViewWidth() const { return ui->widgetGears->sizeHint().width(); }

void ArmourSetView::setGearViewWidth(int width)
{
    ui->widgetGears->setMaximumWidth(width);
    ui->widgetGears->setMinimumWidth(width);
}

int ArmourSetView::getCellViewWidth() const { return ui->widgetCells->sizeHint().width(); }

void ArmourSetView::setCellViewWidth(int width)
{
    ui->widgetCells->setMaximumWidth(width);
    ui->widgetCells->setMinimumWidth(width);
}

void ArmourSetView::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    for (const auto& skillLabel : uniqueSkillLabels)
        setElidedSkill(skillLabel.first, skillLabel.second);
}

void ArmourSetView::showContextMenu(const QPoint& pos)
{
    QMenu contextMenu(getTranslation(dict, "loadout_export_contextmenu"), this);

    auto save =
        contextMenu.addAction(getTranslation(dict, "loadout_save"));
    connect(save, &QAction::triggered, [this](bool) { emit saveSet(armourSet); });

    util::addArmoursetExportToContextMenu(contextMenu, armourSet, dict, options, armoury);
    contextMenu.exec(mapToGlobal(pos));
}

void ArmourSetView::setElidedSkill(QLabel* label, const QString& text)
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