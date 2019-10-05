#include "ui/Loadouts.hpp"
#include "ui/LoadoutEntry.hpp"
#include "ui_Loadouts.h"
#include "util/Translation.hpp"
#include <QSettings>
#include <iostream>

#define GEOMETRY "Loadouts/Geometry"

Loadouts::Loadouts(Options& options, const Dictionary& dict, const Gear::Armoury& armoury,
                   QWidget* parent)
    : QWidget(parent), ui(new Ui::Loadouts), options_(options), dict_(dict), armoury_(armoury)
{
    ui->setupUi(this);
    QSettings settings;
    restoreGeometry(settings.value(GEOMETRY).toByteArray());
    setWindowTitle(getTranslation(dict_, "loadout_title"));
    for (const auto& loadout : options_.myLoadouts)
    {
        addArmourSet(loadout.name, loadout.set);
    }
    connect(ui->setListWidget, &QListWidget::currentItemChanged,
            [this](QListWidgetItem* current, QListWidgetItem* previous) {
                if (lastSelectedSet_ != nullptr)
                {
                    ui->horizontalLayout->removeWidget(lastSelectedSet_);
                    delete lastSelectedSet_;
                    lastSelectedSet_ = nullptr;
                }
                auto entry = dynamic_cast<LoadoutEntry*>(ui->setListWidget->itemWidget(current));
                lastSelectedSet_ = new DetailedArmourSetView(entry->getLoadout(), dict_, armoury_);
                ui->horizontalLayout->addWidget(lastSelectedSet_);
                // TODO: DetailedArmourSetView should have a function change set, so that it doesn't
                // need to be recreated every time
            });
    if (!options_.myLoadouts.empty())
        ui->setListWidget->setCurrentRow(0);
}

Loadouts::~Loadouts() 
{
    save();
    delete ui; 
}

void Loadouts::addArmourSet(const std::string& name, const Gear::ArmourSet& set)
{
    auto widget = new LoadoutEntry(name, set, dict_, options_, armoury_);
    auto item = new QListWidgetItem();
    item->setSizeHint(widget->sizeHint());
    ui->setListWidget->addItem(item);
    ui->setListWidget->setItemWidget(item, widget);
    connect(widget, &LoadoutEntry::remove,
            [item, this]() { delete ui->setListWidget->takeItem(ui->setListWidget->row(item)); });
}

void Loadouts::saveSet(const Gear::ArmourSet& set)
{
    addArmourSet(dict_.getTranslationFor("loadout_newloadout"), set);
    ui->setListWidget->setCurrentRow(ui->setListWidget->count() - 1);
}

void Loadouts::closeEvent(QCloseEvent* event)
{
    QSettings settings;
    settings.setValue(GEOMETRY, saveGeometry());
    QWidget::closeEvent(event);
}

void Loadouts::save()
{
    options_.myLoadouts.clear();
    options_.myLoadouts.reserve(ui->setListWidget->count());
    for (int i = 0; i < ui->setListWidget->count(); ++i)
    {
        QListWidgetItem* item = ui->setListWidget->item(i);
        auto entry = dynamic_cast<LoadoutEntry*>(ui->setListWidget->itemWidget(item));
        options_.myLoadouts.emplace_back(entry->getName(), entry->getLoadout());
    }
}
