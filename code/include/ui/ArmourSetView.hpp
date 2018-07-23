#ifndef DAUNTLESS_ASS_UI_ARMOURSETVIEW_HPP
#define DAUNTLESS_ASS_UI_ARMOURSETVIEW_HPP

#include "gear/Armoury.hpp"
#include "Dictionary.hpp"
#include "gear/ArmourSet.hpp"
#include <QWidget>

namespace Ui
{
class ArmourSetView;
}

class ArmourSetView : public QWidget
{
    Q_OBJECT

  public:
    explicit ArmourSetView(const Dictionary &dict, const Gear::ArmourSet &set, const Gear::Armoury &armoury, QWidget *parent = 0);
    ~ArmourSetView();

  private:
    void addGear(const Gear::Gear &gear);
    void addCell(const std::pair<Gear::Cell, int> &cell);
    void addSkill(const Gear::Skill &skill);

    const Dictionary &dict;
    Ui::ArmourSetView *ui;
};

#endif // !DAUNTLESS_ASS_UI_ARMOURSETVIEW_HPP