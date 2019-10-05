#pragma once

#include <QWidget>

namespace util
{
class HorizontalSpacer : public QWidget
{
  public:
    HorizontalSpacer(QWidget* parent = NULL);
};
class VerticalSpacer: public QWidget
{
  public:
    VerticalSpacer(QWidget* parent = NULL);
};
}