#include "ui/AdvancedSearch.hpp"
#include "ui/Translation.hpp"
#include "ui_AdvancedSearch.h"
#include <sstream>

AdvancedSearch::AdvancedSearch(Gear::WeaponType weaponType, const Gear::Armoury &armoury,
                               const Dictionary &dict, Options &options,
                               std::vector<Gear::Skill> skills, QWidget *parent)
    : QDialog(parent), ui(new Ui::AdvancedSearch), options(options), dict(dict), armoury(armoury),
      wantedSkills(std::move(skills))
{
    ui->setupUi(this);
    setWindowTitle(getTranslation(dict, "title_advanced_search"));
    ui->labelWeapons->setText(getTranslation(dict, "label_weapons"));
    ui->labelHeads->setText(getTranslation(dict, "label_head_armour"));
    ui->labelTorsos->setText(getTranslation(dict, "label_torsos_armour"));
    ui->labelArms->setText(getTranslation(dict, "label_arms_armour"));
    ui->labelLegs->setText(getTranslation(dict, "label_legs_armour"));
    ui->labelCells->setText(getTranslation(dict, "label_cells"));
    ui->pushButtonSearch->setText(getTranslation(dict, "button_search"));
    ui->pushButtonCancel->setText(getTranslation(dict, "button_cancel"));

    for (const auto &weapon : armoury.getWeaponsWithSkill(wantedSkills, weaponType))
    {
        auto checkbox = new QCheckBox();
        if (options.checkedGear.count(weapon.getName()) > 0)
            checkbox->setChecked(options.checkedGear.at(weapon.getName()));
        else
            checkbox->setChecked(true);
        checkbox->setText(getTranslation(dict, weapon.getName()));
        checkbox->setToolTip(QString::fromStdString(weapon.getToolTip(dict)));
        addItem(checkbox, ui->listWidgetWeapons);
        connect(checkbox, &QCheckBox::stateChanged, [&options, weapon](int state) {
            options.checkedGear[weapon.getName()] = (state == Qt::Checked);
        });
        weapons.insert({checkbox, weapon});
    }
    addArmours(Gear::ArmourType::Head, ui->listWidgetHeads);
    addArmours(Gear::ArmourType::Torso, ui->listWidgetTorsos);
    addArmours(Gear::ArmourType::Arms, ui->listWidgetArms);
    addArmours(Gear::ArmourType::Legs, ui->listWidgetLegs);

    
    ui->listWidgetCells->setStyleSheet("QListWidget::item { border-bottom: 1px solid black; }");
    for (const auto &skill : wantedSkills)
    {
        auto layout = new QGridLayout();
        auto widget = new QWidget();
        widget->setLayout(layout);
        auto label = new QLabel();
        label->setText(getTranslation(dict, skill.getName()));
        layout->addWidget(label, 0, 0, 1, 2);
        int row = 1;
        for (int i = 3; i > 0; --i)
        {
            Gear::Cell cell(Gear::Skill(skill.getName(), i),
                            armoury.getSkillInfoFor(skill.getName()).getType());

            auto spinBox = new QSpinBox();
            cells.insert({spinBox, cell});
            layout->addWidget(spinBox, row, 1);
            spinBox->setMinimum(0);
            if (options.cells.count(cell) > 0)
                spinBox->setValue(options.cells.at(cell));
            else
                spinBox->setValue(6 / cell.getSkill().getSkillPoints());

            auto label = new QLabel();
            std::stringstream ss;
            ss << "+" << i;
            label->setText(QString::fromStdString(ss.str()));
            layout->addWidget(label, row, 0);

            ++row;
            connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                    [&options, cell](int value) { options.cells[cell] = value; });
        }
        addItem(widget, ui->listWidgetCells);
    }


    ui->listWidgetWeapons->setMinimumWidth(ui->listWidgetWeapons->sizeHintForColumn(0));
    ui->listWidgetHeads->setMinimumWidth(ui->listWidgetHeads->sizeHintForColumn(0));
    ui->listWidgetTorsos->setMinimumWidth(ui->listWidgetTorsos->sizeHintForColumn(0));
    ui->listWidgetArms->setMinimumWidth(ui->listWidgetArms->sizeHintForColumn(0));
    ui->listWidgetLegs->setMinimumWidth(ui->listWidgetLegs->sizeHintForColumn(0));
    ui->listWidgetCells->setMinimumWidth(ui->listWidgetCells->sizeHintForColumn(0));

    connect(ui->pushButtonCancel, &QPushButton::clicked, [this]() { quit(); });
    connect(ui->pushButtonSearch, &QPushButton::clicked, [this]() { search(); });
}

AdvancedSearch::~AdvancedSearch() { delete ui; }

void AdvancedSearch::quit() { delete this; }

void AdvancedSearch::addItem(QWidget *widget, QListWidget *list)
{
    auto item = new QListWidgetItem();
    item->setSizeHint(widget->sizeHint());
    list->addItem(item);
    list->setItemWidget(item, widget);
}

void AdvancedSearch::addArmours(Gear::ArmourType type, QListWidget *list)
{
    for (const auto &armour : armoury.getArmourWithSkill(wantedSkills, type))
    {
        auto checkbox = new QCheckBox();
        if (options.checkedGear.count(armour.getName()) > 0)
            checkbox->setChecked(options.checkedGear.at(armour.getName()));
        else
            checkbox->setChecked(true);
        checkbox->setText(getTranslation(dict, armour.getName()));
        checkbox->setToolTip(QString::fromStdString(armour.getToolTip(dict)));
        addItem(checkbox, list);
        connect(checkbox, &QCheckBox::stateChanged, [this, armour](int state) {
            options.checkedGear[armour.getName()] = (state == Qt::Checked);
        });
        armours[type].insert({checkbox, armour});
    }
}

std::vector<Gear::Armour> AdvancedSearch::getArmour(Gear::ArmourType type)
{
    std::vector<Gear::Armour> armours;
    for (const auto &armour : this->armours[type])
        if (armour.first->isChecked())
            armours.push_back(armour.second);
    return armours;
}

void AdvancedSearch::search()
{
    std::vector<Gear::Weapon> weapons;
    for (const auto &weapon : this->weapons)
        if (weapon.first->isChecked())
            weapons.push_back(weapon.second);
    std::vector<Gear::Armour> heads = getArmour(Gear::ArmourType::Head);
    std::vector<Gear::Armour> torsos = getArmour(Gear::ArmourType::Torso);
    std::vector<Gear::Armour> arms = getArmour(Gear::ArmourType::Arms);
    std::vector<Gear::Armour> legs = getArmour(Gear::ArmourType::Legs);
    Gear::CellList cells;
    for (const auto &cell : this->cells)
        cells += cell.second * cell.first->value();
    emit(armourSetSearch(ArmourSetSearch(weapons, heads, torsos, arms, legs, wantedSkills, cells)));
    quit();
}

void AdvancedSearch::closeEvent(QCloseEvent *event) { quit(); }