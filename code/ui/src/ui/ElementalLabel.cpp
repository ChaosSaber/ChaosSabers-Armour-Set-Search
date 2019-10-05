#include "ui/ElementalLabel.hpp"
#include "util/Translation.hpp"
#include "ui_ElementalLabel.h"
#include <iostream>

ElementalLabel::ElementalLabel(Gear::Element element, bool isStrength, const Dictionary& dict,
                               QWidget* parent)
    : QFrame(parent), ui(new Ui::ElementalLabel)
{
    ui->setupUi(this);
    if (isStrength)
    {
        ui->signLabel->setText("+");
        setStyleSheet("color: green;");
    }
    else
    {
        ui->signLabel->setText("-");
        setStyleSheet("color: red;");
    }
    switch (element)
    {
    case Gear::Element::Fire:
        ui->elementalName->setText(getTranslation(dict, "element_fire"));
        ui->iconLabel->setPixmap(QPixmap(":/images/icons/elements/Blaze.png"));
        break;
    case Gear::Element::Ice:
        ui->elementalName->setText(getTranslation(dict, "element_ice"));
        ui->iconLabel->setPixmap(QPixmap(":/images/icons/elements/Frost.png"));
        break;
    case Gear::Element::Radiant:
        ui->elementalName->setText(getTranslation(dict, "element_radiant"));
        ui->iconLabel->setPixmap(QPixmap(":/images/icons/elements/Radiant.png"));
        break;
    case Gear::Element::Umbral:
        ui->elementalName->setText(getTranslation(dict, "element_umbral"));
        ui->iconLabel->setPixmap(QPixmap(":/images/icons/elements/Umbral.png"));
        break;
    case Gear::Element::Terra:
        ui->elementalName->setText(getTranslation(dict, "element_terra"));
        ui->iconLabel->setPixmap(QPixmap(":/images/icons/elements/Terra.png"));
        break;
    case Gear::Element::Shock:
        ui->elementalName->setText(getTranslation(dict, "element_shock"));
        ui->iconLabel->setPixmap(QPixmap(":/images/icons/elements/Shock.png"));
        break;
    default: std::cout << "Warning: unknown element type: " << element << std::endl;
    }
}

ElementalLabel::~ElementalLabel() { delete ui; }
