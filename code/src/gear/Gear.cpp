#include "gear/Gear.hpp"

Gear::Gear::Gear(std::string name, std::vector<int> tiers)
    : name(std::move(name)), tiers(std::move(tiers))
{
}

const std::string &Gear::Gear::getName() const { return name; }

const std::vector<int> &Gear::Gear::getTiers() const { return tiers; }
