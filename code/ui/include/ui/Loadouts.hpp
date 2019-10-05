#pragma once

#include "Dictionary.hpp"
#include "Options.hpp"
#include "gear/Armoury.hpp"
#include <QWidget>
#include <gear/ArmourSet.hpp>
#include <string>
#include <ui/DetailedArmourSetView.hpp>
#include <unordered_map>

namespace Ui
{
class Loadouts;
}

class Loadouts : public QWidget
{
  public:
    Loadouts(Options& options, const Dictionary& dict, const Gear::Armoury& armoury,
             QWidget* parent = NULL);
    ~Loadouts();

    /**
     * Adds a new Armourset to the Loadouts.
     * @param name The name of the new loadout.
     * @param set The new loadout.
     */
    void addArmourSet(const std::string& name, const Gear::ArmourSet& set);

    /**
     * saves a new ArmourSet to the Loadouts. The new set will be named unnamed and automatically
     * selected.
     * @param set The new loadout to save.
     */
    void saveSet(const Gear::ArmourSet& set);

    void closeEvent(QCloseEvent* event) override;

  private:
    void save();
    Ui::Loadouts* ui;
    Options& options_;
    const Dictionary& dict_;
    const Gear::Armoury& armoury_;
    DetailedArmourSetView* lastSelectedSet_ = nullptr;
};
