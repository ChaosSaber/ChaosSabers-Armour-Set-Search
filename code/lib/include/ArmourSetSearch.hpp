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
                    const Gear::WantedSkillList& wantedSkills, const Options& options);
    ArmourSetSearch(std::vector<Gear::Weapon> weapons, std::vector<Gear::Armour> heads,
                    std::vector<Gear::Armour> torsos, std::vector<Gear::Armour> arms,
                    std::vector<Gear::Armour> legs, const Gear::WantedSkillList& wantedSkills);

    void search(const Gear::Armoury& armoury, const bool* cancel);
    const std::vector<Gear::ArmourSet>& getArmourSets() const;
    std::vector<Gear::ArmourSet>&& moveArmourSets();
    void setAvaiableCells(Gear::AvailableCellList&& availableCells);

    /**
     * Sets the progress callback which is called from the search process after every percent
     * progress. Make sure this function is threadsafe, because it can happen, that it is called
     * simultaneously from multiple threads. Best is also to forward the actual execution to it's
     * own thread for maximum performance.
     * @param progress The callback.
     */
    void setProgressCallback(ProgressCallBack progress);
    const Gear::WantedSkillList& getWantedSkills() const;

  private:
    /**
     * checks if the given set fullfills the skill requirements
     * @parameter set The set to check
     * @parameter armoury A reference to an armoury used for checking of skilltypes
     * @parameter cells A list of cells which are available to put into the set
     */
    void checkSet(Gear::ArmourSet& set, const Gear::Armoury& armoury,
                  const Gear::AvailableCellList& cells);
    /**
     * adds an armour to the list of found armoursets (thred safe)
     * @param set: The set to add
     */
    void addArmourSet(const Gear::ArmourSet& set);

    std::vector<Gear::ArmourSet> armourSets;
    std::vector<Gear::Armour> heads, torsos, arms, legs;
    std::vector<Gear::Weapon> weapons;
    Gear::WantedSkillList wantedSkills;

    Gear::AvailableCellList availableCells;
    struct SearchStatistics stats;

    std::mutex addMutex;

    ProgressCallBack progress;
};

#endif // !DAUNTLESS_ASS_ARMOURSETSEARCH_HPP