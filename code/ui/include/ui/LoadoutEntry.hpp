#pragma once

#include "gear/ArmourSet.hpp"
#include <string>
#include <QWidget>
#include <QLabel>
#include "Dictionary.hpp"
#include "Options.hpp"
#include "gear/Armoury.hpp"

class LoadoutEntry : public QWidget
{
    Q_OBJECT
signals:
    void remove();
  public:
    LoadoutEntry(const std::string& name, const Gear::ArmourSet& set, const Dictionary& dict,
                 Options& options, const Gear::Armoury& armoury,
                 QWidget* parent = NULL);
    const std::string& getName() const;
    const Gear::ArmourSet& getLoadout() const;

  private:
    void showContextMenu(const QPoint& pos);

    std::string name_;
    Gear::ArmourSet set_;
    const Dictionary& dict_;
    Options& options_;
    const Gear::Armoury& armoury_;

    QLabel* nameLabel_ = nullptr;
};