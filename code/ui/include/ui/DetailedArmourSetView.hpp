#pragma once

#include "gear/ArmourSet.hpp"
#include <QWidget>

namespace Ui
{
class DetailedArmourSetView;
}

class DetailedArmourSetView : public QWidget
{
  public:
    DetailedArmourSetView(const Gear::ArmourSet& set, const Dictionary& dict,
                          const Gear::Armoury& armoury, QWidget* parent = NULL);
    ~DetailedArmourSetView();

  private:
    Ui::DetailedArmourSetView* ui;
};