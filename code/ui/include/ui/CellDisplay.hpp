#pragma once

#include "gear/Armoury.hpp"
#include "Dictionary.hpp"
#include "gear/Cell.hpp"
#include <QWidget>

namespace Ui
{
class CellDisplay;
}

class CellDisplay : public QWidget
{
    Q_OBJECT
  public:
    CellDisplay(const Gear::Cell& cell, const Dictionary& dict, const Gear::Armoury& armoury, QWidget* parent = NULL);
    ~CellDisplay();

  private:
    Ui::CellDisplay* ui;
};