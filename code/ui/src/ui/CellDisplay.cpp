#include "ui/CellDisplay.hpp"
#include "ui_CellDisplay.h"
#include <iostream>

CellDisplay::CellDisplay(const Gear::Cell& cell, const Dictionary& dict,
                         const Gear::Armoury& armoury, QWidget* parent)
    : QWidget(parent), ui(new Ui::CellDisplay)
{
    ui->setupUi(this);
    ui->nameLabel->setText(QString::fromStdString(cell.getCellInfo(dict, armoury)));
    switch (cell.getCellType())
    {
    case Gear::SkillType::Defense:
        ui->imageLabel->setPixmap(QPixmap(
            QString::fromStdString(":/images/icons/cells/100x100/defence+" +
                                   std::to_string(cell.getSkill().getSkillPoints()) + ".png")));
        break;
    case Gear::SkillType::Mobility:
        ui->imageLabel->setPixmap(QPixmap(
            QString::fromStdString(":/images/icons/cells/100x100/mobility+" +
                                   std::to_string(cell.getSkill().getSkillPoints()) + ".png")));
        break;
    case Gear::SkillType::Power:
        ui->imageLabel->setPixmap(QPixmap(
            QString::fromStdString(":/images/icons/cells/100x100/power+" +
                                   std::to_string(cell.getSkill().getSkillPoints()) + ".png")));
        break;
    case Gear::SkillType::Technique:
        ui->imageLabel->setPixmap(QPixmap(
            QString::fromStdString(":/images/icons/cells/100x100/technique+" +
                                   std::to_string(cell.getSkill().getSkillPoints()) + ".png")));
        break;
    case Gear::SkillType::Utility:
        ui->imageLabel->setPixmap(QPixmap(
            QString::fromStdString(":/images/icons/cells/100x100/utility+" +
                                   std::to_string(cell.getSkill().getSkillPoints()) + ".png")));
        break;
    default: std::cout << "unknown Skilltype" << cell.getCellType() << std::endl; break;
    }
}
CellDisplay::~CellDisplay() { delete ui; }
