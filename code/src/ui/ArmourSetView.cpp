#include "ui/ArmourSetView.hpp"
#include "ui/Translation.hpp"
#include "ui_ArmourSetView.h"
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
      scrollBarWidth(scrollBarWidth), armourSet(set)
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
    label->setToolTip(QString::fromStdString(gear.getToolTip(dict)));
    ui->verticalLayoutGearParts->addWidget(label);
}

void ArmourSetView::addCell(const std::pair<Gear::Cell, int>& cell)
{
    QLabel* label = new QLabel();
    std::stringstream ss;
    ss << cell.second << "x " << cell.first.getCellInfo(dict);
    label->setText(QString::fromStdString(ss.str()));
    ui->verticalLayoutGearCells->addWidget(label);
}

void ArmourSetView::addSkill(const Gear::Skill& skill)
{
    QLabel* label = new QLabel();
    label->setText(QString::fromStdString(skill.toString(dict)));
    ui->verticalLayoutSkills->addWidget(label);
}

void ArmourSetView::exportTextToClipBoard(const std::string& text) const
{
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(QString::fromStdString(text));
}

void ArmourSetView::exportTextToFile(const std::string& text) const
{
    auto fileName = QFileDialog::getSaveFileName(nullptr, getTranslation(dict, "menu_save"),
                                                 options.lastExportTextSaveLocation);
    if (fileName.isEmpty())
        return;
    QFileInfo info(fileName);
    options.lastExportTextSaveLocation = info.path();
    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly))
    {
        std::stringstream ss;
        ss << "Couldn't open file \"" << fileName.toStdString() << "\"";
        QMessageBox box(QMessageBox::Critical, getTranslation(dict, "error"),
                        QString::fromStdString(ss.str()));
        return;
    }
    file.write(text.c_str());
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
    QMenu contextMenu(getTranslation(dict, "export_contextmenu"), this);

    // auto contextMenuText = contextMenu.addMenu(getTranslation(dict, "export_text"));
    // auto actionTextToClipBoard =
    //    contextMenuText->addAction(getTranslation(dict, "export_to_clipboard"));
    // connect(actionTextToClipBoard, &QAction::triggered,
    //        [this](bool) { exportTextToClipBoard(armourSet.exportToText(dict)); });
    // auto actionTextToFile = contextMenuText->addAction(getTranslation(dict, "export_to_file"));
    // connect(actionTextToFile, &QAction::triggered,
    //        [this](bool) { exportTextToFile(armourSet.exportToText(dict)); });
    {
        auto contextMenuText = contextMenu.addMenu(getTranslation(dict, "export_text1"));
        auto actionTextToClipBoard =
            contextMenuText->addAction(getTranslation(dict, "export_to_clipboard"));
        connect(actionTextToClipBoard, &QAction::triggered,
                [this](bool) { exportTextToClipBoard(armourSet.exportToText(dict)); });
        auto actionTextToFile = contextMenuText->addAction(getTranslation(dict, "export_to_file"));
        connect(actionTextToFile, &QAction::triggered,
                [this](bool) { exportTextToFile(armourSet.exportToText(dict)); });
    }
    {
        auto contextMenuText = contextMenu.addMenu(getTranslation(dict, "export_text2"));
        auto actionTextToClipBoard =
            contextMenuText->addAction(getTranslation(dict, "export_to_clipboard"));
        connect(actionTextToClipBoard, &QAction::triggered,
                [this](bool) { exportTextToClipBoard(armourSet.exportToText2(dict)); });
        auto actionTextToFile = contextMenuText->addAction(getTranslation(dict, "export_to_file"));
        connect(actionTextToFile, &QAction::triggered,
                [this](bool) { exportTextToFile(armourSet.exportToText2(dict)); });
    }
    {
        auto contextMenuText = contextMenu.addMenu(getTranslation(dict, "export_text3"));
        auto actionTextToClipBoard =
            contextMenuText->addAction(getTranslation(dict, "export_to_clipboard"));
        connect(actionTextToClipBoard, &QAction::triggered,
                [this](bool) { exportTextToClipBoard(armourSet.exportToText3(dict)); });
        auto actionTextToFile = contextMenuText->addAction(getTranslation(dict, "export_to_file"));
        connect(actionTextToFile, &QAction::triggered,
                [this](bool) { exportTextToFile(armourSet.exportToText3(dict)); });
    }

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