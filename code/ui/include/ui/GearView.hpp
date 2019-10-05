#pragma once

#include "gear/Armoury.hpp"
#include "Dictionary.hpp"
#include "gear/Gear.hpp"
#include <QWidget>

namespace Ui
{
class GearView;
}

class GearView : public QWidget
{
    Q_OBJECT
  public:
    GearView(const Gear::Gear& gear, const Dictionary& dict, const Gear::Armoury& armoury, QWidget* parent = NULL);
    ~GearView();

  private:
    Ui::GearView* ui;
};