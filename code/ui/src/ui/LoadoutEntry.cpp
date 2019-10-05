#include "ui/LoadoutEntry.hpp"
#include "util/Export.hpp"
#include "util/Spacer.hpp"
#include "util/Translation.hpp"
#include <QInputDialog>
#include <QLayout>
#include <QMenu>
#include <QMessageBox>
#include <iostream>

LoadoutEntry::LoadoutEntry(const std::string& name, const Gear::ArmourSet& set,
                           const Dictionary& dict, Options& options, const Gear::Armoury& armoury,
                           QWidget* parent)
    : QWidget(parent), name_(name), set_(set), dict_(dict), options_(options), armoury_(armoury)
{
    auto* layout = new QHBoxLayout;
    setLayout(layout);
    auto* iconLabel = new QLabel;
    switch (set.getWeapon().getType())
    {
    case Gear::WeaponType::AetherStrikers:
        iconLabel->setPixmap(QPixmap(":/images/icons/weapontypes/30x30/aether-strikers.png"));
        break;
    case Gear::WeaponType::Axe:
        iconLabel->setPixmap(QPixmap(":/images/icons/weapontypes/30x30/axe.png"));
        break;
    case Gear::WeaponType::Chainblades:
        iconLabel->setPixmap(QPixmap(":/images/icons/weapontypes/30x30/chain-blades.png"));
        break;
    case Gear::WeaponType::Hammer:
        iconLabel->setPixmap(QPixmap(":/images/icons/weapontypes/30x30/hammer.png"));
        break;
    case Gear::WeaponType::Pike:
        iconLabel->setPixmap(QPixmap(":/images/icons/weapontypes/30x30/war-pike.png"));
        break;
    case Gear::WeaponType::Reapeater:
        iconLabel->setPixmap(QPixmap(":/images/icons/weapontypes/30x30/repeater.png"));
        break;
    case Gear::WeaponType::Sword:
        iconLabel->setPixmap(QPixmap(":/images/icons/weapontypes/30x30/sword.png"));
        break;
    default: std::cout << "unknown weapon type: " << set.getWeapon().getType() << std::endl; break;
    }
    layout->addWidget(iconLabel);
    nameLabel_ = new QLabel(QString::fromStdString(name));
    auto font = nameLabel_->font();
    font.setPointSize(12);
    nameLabel_->setFont(font);
    layout->addWidget(nameLabel_);
    layout->addWidget(new util::HorizontalSpacer);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QWidget::customContextMenuRequested,
            [this](const QPoint& pos) { showContextMenu(pos); });
}

const std::string& LoadoutEntry::getName() const { return name_; }

const Gear::ArmourSet& LoadoutEntry::getLoadout() const { return set_; }

void LoadoutEntry::showContextMenu(const QPoint& pos)
{
    QMenu contextMenu(getTranslation(dict_, "loadout_export_contextmenu"), this);

    auto save = contextMenu.addAction(getTranslation(dict_, "loadoutentry_delete"));
    connect(save, &QAction::triggered, [this](bool) {
        QMessageBox mbox(this);
        mbox.setText(getTranslation(dict_, "loadoutentry_delete_confirmationboxtitle"));
        mbox.setInformativeText(getTranslation(dict_, "loadoutentry_delete_confirmationboxtext"));
        mbox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        mbox.setDefaultButton(QMessageBox::Cancel);
        if (mbox.exec() == QMessageBox::Ok)
            emit remove();
    });

    auto rename = contextMenu.addAction(getTranslation(dict_, "loadoutentry_rename"));
    connect(rename, &QAction::triggered, [this](bool) {
        bool ok;
        QString text = QInputDialog::getText(
            this, getTranslation(dict_, "loadoutentry_rename_inputdialog_title"),
            getTranslation(dict_, "loadoutentry_rename_inputdialog_text"), QLineEdit::Normal,
            nameLabel_->text(), &ok);
        if (ok && !text.isEmpty())
        {
            name_ = text.toStdString();
            nameLabel_->setText(text);
        }
    });

    util::addArmoursetExportToContextMenu(contextMenu, set_, dict_, options_, armoury_);
    contextMenu.exec(mapToGlobal(pos));
}
