#ifndef DAUNTLESS_ASS_ARMOURSETSEARCH_HPP
#define DAUNTLESS_ASS_ARMOURSETSEARCH_HPP

#include "Dictionary.hpp"
#include "Options.hpp"
#include "gear/ArmourSet.hpp"
#include "gear/Armoury.hpp"
#include <atomic>
#include <chrono>
#include <functional>
#include <vector>

class ArmourSetSearch
{
  public:
    ArmourSetSearch(const Gear::Armoury& armoury, Gear::WeaponType weaponType,
                    std::vector<Gear::Skill> skills, const Options& options,
                    Gear::CellList availableCells = Gear::CellList());
    ArmourSetSearch(std::vector<Gear::Weapon> weapons, std::vector<Gear::Armour> heads,
                    std::vector<Gear::Armour> torsos, std::vector<Gear::Armour> arms,
                    std::vector<Gear::Armour> legs, std::vector<Gear::Skill> wantedSkills,
                    Gear::CellList availableCells = Gear::CellList());

    struct SearchStatistics
    {
        std::atomic<size_t> possibleCombinations;
        std::atomic<size_t> searchedCombinations;
        std::atomic<size_t> foundSets;
        std::atomic<int> progress;
        std::atomic<std::chrono::time_point<std::chrono::high_resolution_clock>> start, end;
        SearchStatistics(const SearchStatistics& other)
        {
            possibleCombinations.store(other.possibleCombinations.load());
            searchedCombinations.store(other.searchedCombinations.load());
            foundSets.store(other.foundSets.load());
            progress.store(other.progress.load());
            start.store(other.start.load());
            end.store(other.end.load());
        }
        SearchStatistics()
        {
            possibleCombinations = 0;
            searchedCombinations = 0;
            foundSets = 0;
            progress = 0;
            start = std::chrono::high_resolution_clock::now();
            end = std::chrono::high_resolution_clock::now();
        }
    };

    void search(const Gear::Armoury& armoury, bool* cancel,
                std::function<void(SearchStatistics)> progress);
    std::string toString(const Dictionary& dict);
    const std::vector<Gear::ArmourSet>& getArmourSets() const;
    std::vector<Gear::ArmourSet>&& moveArmourSets();
    void setAvaiableCells(Gear::CellList availableCells);
    const std::vector<Gear::Skill>& getWantedSkills() const;

  private:
    void checkSet(Gear::ArmourSet set, const Gear::Armoury& armoury);

    std::vector<Gear::ArmourSet> armourSets;
    std::vector<Gear::Armour> heads, torsos, arms, legs;
    std::vector<Gear::Weapon> weapons;
    std::vector<Gear::Skill> wantedSkills;
    Gear::CellList availableCells;
    struct SearchStatistics stats;
};

#endif // !DAUNTLESS_ASS_ARMOURSETSEARCH_HPP