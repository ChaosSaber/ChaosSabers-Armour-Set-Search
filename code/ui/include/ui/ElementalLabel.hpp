#pragma once

#include "Dictionary.hpp"
#include "gear/Elements.hpp"
#include <QFrame>

namespace Ui
{
class ElementalLabel;
}

class ElementalLabel : public QFrame
{
    Q_OBJECT
  public:
    ElementalLabel(Gear::Element element, bool isStrength, const Dictionary& dict,
                   QWidget* parent = NULL);
    ~ElementalLabel();

  private:
    Ui::ElementalLabel* ui;
};