#ifndef DAUNTLESS_ASS_UI_ADVANCEDSEARCH_HPP
#define DAUNTLESS_ASS_UI_ADVANCEDSEARCH_HPP

#include "ArmourSetSearch.hpp"
#include "Dictionary.hpp"
#include "Options.hpp"
#include "gear/Armoury.hpp"
#include <QCheckBox>
#include <QDialog>
#include <QListWidget>
#include <QSpinBox>

namespace Ui
{
class AdvancedSearch;
}

// TODO: change with QDialog: http://doc.qt.io/qt-5/qdialog.html#details
class AdvancedSearch : public QDialog
{
    Q_OBJECT
  public:
    AdvancedSearch(Gear::WeaponType weaponType, const Gear::Armoury &armoury,
                   const Dictionary &dict, Options &options, std::vector<Gear::Skill> wantedSkills,
                   QWidget *parent = NULL);
    ~AdvancedSearch();
    void set(Gear::WeaponType weaponType, std::vector<Gear::Skill> wantedSkills);
  signals:
    void armourSetSearch(ArmourSetSearch *ass);

  private:
    void addItem(QWidget *widget, QListWidget *list);
    void addArmours(Gear::ArmourType type, QListWidget *list);
    void search();
    std::vector<Gear::Armour> getArmour(Gear::ArmourType type);

    const Dictionary &dict;
    const Gear::Armoury &armoury;
    Options &options;
    std::vector<Gear::Skill> wantedSkills;

    std::unordered_map<QCheckBox *, Gear::Weapon> weapons;
    std::unordered_map<Gear::ArmourType, std::unordered_map<QCheckBox *, Gear::Armour>> armours;
    std::unordered_map<QSpinBox *, Gear::Cell> cells;

    Ui::AdvancedSearch *ui;
};

#endif // !DAUNTLESS_ASS_UI_ADVANCEDSEARCH_HPP