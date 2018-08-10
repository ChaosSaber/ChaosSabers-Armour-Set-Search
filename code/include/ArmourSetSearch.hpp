#ifndef DAUNTLESS_ASS_ARMOURSETSEARCH_HPP
#define DAUNTLESS_ASS_ARMOURSETSEARCH_HPP

#include "Dictionary.hpp"
#include "Options.hpp"
#include "gear/ArmourSet.hpp"
#include "gear/Armoury.hpp"
#include <functional>
#include <vector>

class ArmourSetSearch
{
  public:
    ArmourSetSearch(const Gear::Armoury &armoury, Gear::WeaponType weaponType,
                    std::vector<Gear::Skill> skills, const Options &options,
                    Gear::CellList availableCells = Gear::CellList());
    ArmourSetSearch(std::vector<Gear::Weapon> weapons, std::vector<Gear::Armour> heads,
                    std::vector<Gear::Armour> torsos, std::vector<Gear::Armour> arms,
                    std::vector<Gear::Armour> legs, std::vector<Gear::Skill> wantedSkills,
                    Gear::CellList availableCells = Gear::CellList());
    void search(const Gear::Armoury &armoury, bool *cancel, std::function<void(int)> progress);
    std::string toString(const Dictionary &dict);
    const std::vector<Gear::ArmourSet> &getArmourSets() const;
    std::vector<Gear::ArmourSet> &&moveArmourSets();
    void setAvaiableCells(Gear::CellList availableCells);
    const std::vector<Gear::Skill> &getWantedSkills() const;

  private:
    void checkSet(Gear::ArmourSet set, const Gear::Armoury &armoury);

    std::vector<Gear::ArmourSet> armourSets;
    std::vector<Gear::Armour> heads, torsos, arms, legs;
    std::vector<Gear::Weapon> weapons;
    std::vector<Gear::Skill> wantedSkills;
    Gear::CellList availableCells;
};

#endif // !DAUNTLESS_ASS_ARMOURSETSEARCH_HPP