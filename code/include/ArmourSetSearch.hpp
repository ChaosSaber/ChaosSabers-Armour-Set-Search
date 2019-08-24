#ifndef DAUNTLESS_ASS_ARMOURSETSEARCH_HPP
#define DAUNTLESS_ASS_ARMOURSETSEARCH_HPP

#include "Dictionary.hpp"
#include "Options.hpp"
#include "gear/ArmourSet.hpp"
#include "gear/Armoury.hpp"
#include <atomic>
#include <chrono>
#include <functional>
#include <mutex>
#include <vector>

class ArmourSetSearch
{
  public:
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

    typedef std::function<void(SearchStatistics)> ProgressCallBack;
    ArmourSetSearch(const Gear::Armoury& armoury, Gear::WeaponType weaponType,
                    std::vector<Gear::Skill> skills, const Options& options,
                    ProgressCallBack progress = [](SearchStatistics) {},
                    Gear::CellList availableCells = Gear::CellList());
    ArmourSetSearch(std::vector<Gear::Weapon> weapons, std::vector<Gear::Armour> heads,
                    std::vector<Gear::Armour> torsos, std::vector<Gear::Armour> arms,
                    std::vector<Gear::Armour> legs, std::vector<Gear::Skill> wantedSkills,
                    ProgressCallBack progress = [](SearchStatistics) {},
                    Gear::CellList availableCells = Gear::CellList());

    void search(const Gear::Armoury& armoury, const bool* cancel);
    const std::vector<Gear::ArmourSet>& getArmourSets() const;
    std::vector<Gear::ArmourSet>&& moveArmourSets();
    void setAvaiableCells(Gear::CellList availableCells);
    void setProgressCallback(ProgressCallBack progress);
    const std::vector<Gear::Skill>& getWantedSkills() const;

  private:
    /**
     * checks if the given set fullfills the skill requirements
     * @parameter set The set to check
     * @parameter armoury A reference to an armoury used for checking of skilltypes
     * @parameter cells A list of cells which are available to put into the set
     */
    void checkSet(Gear::ArmourSet& set, const Gear::Armoury& armoury, Gear::CellList& cells);
    /**
     * adds an armour to the list of found armoursets (thred safe)
     * @param set: The set to add
     */
    void addArmourSet(const Gear::ArmourSet& set);
    void reset();

    std::vector<Gear::ArmourSet> armourSets;
    std::vector<Gear::Armour> heads, torsos, arms, legs;
    std::vector<Gear::Weapon> weapons;
    std::vector<Gear::Skill> wantedSkills;
    std::size_t indexWeapon, indexHead, indexTorso, indexArm, indexLeg;

    Gear::CellList availableCells;
    struct SearchStatistics stats;

    std::mutex addMutex;
    std::mutex getArmourSetMutex;

    ProgressCallBack progress;
};

#endif // !DAUNTLESS_ASS_ARMOURSETSEARCH_HPP