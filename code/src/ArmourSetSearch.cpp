#include "ArmourSetSearch.hpp"
#include "util/GreyCodeGenerator.hpp"
#include <iostream>
#include <sstream>
#include <thread>

ArmourSetSearch::ArmourSetSearch(const Gear::Armoury& armoury, Gear::WeaponType weaponType,
                                 const Gear::WantedSkillList& skills, const Options& options)
    : ArmourSetSearch(armoury.getWeaponsWithSkill(skills, weaponType, options),
                      armoury.getArmourWithSkill(skills, Gear::Head, options),
                      armoury.getArmourWithSkill(skills, Gear::Torso, options),
                      armoury.getArmourWithSkill(skills, Gear::Arms, options),
                      armoury.getArmourWithSkill(skills, Gear::Legs, options), skills)
{
}

ArmourSetSearch::ArmourSetSearch(std::vector<Gear::Weapon> weapons, std::vector<Gear::Armour> heads,
                                 std::vector<Gear::Armour> torsos, std::vector<Gear::Armour> arms,
                                 std::vector<Gear::Armour> legs,
                                 const Gear::WantedSkillList& wantedSkills)
    : weapons(std::move(weapons)), heads(std::move(heads)), torsos(std::move(torsos)),
      arms(std::move(arms)), legs(std::move(legs)), wantedSkills(wantedSkills)
{
}

void ArmourSetSearch::search(const Gear::Armoury& armoury, const bool* cancel)
{
    if (heads.size() == 0 || torsos.size() == 0 || arms.size() == 0 || legs.size() == 0 ||
        weapons.size() == 0)
    {
        std::cout << "Did not start search because at least one gear list was empty" << std::endl;
        return;
    }
    // may return 0 when not able to detect
    auto concurentThreadsSupported = std::thread::hardware_concurrency();
    if (concurentThreadsSupported == 0)
        concurentThreadsSupported = 1;
    std::cout << "searching armoursets with " << concurentThreadsSupported << " threads"
              << std::endl;
    // TODO: maximum number of found sets to prevent out of memory
    stats.possibleCombinations =
        heads.size() * torsos.size() * arms.size() * legs.size() * weapons.size();
    stats.searchedCombinations = 0;
    stats.foundSets = 0;
    stats.progress = 0;
    stats.start = std::chrono::high_resolution_clock::now();
    stats.end = std::chrono::high_resolution_clock::now();
    progress(stats);
    std::mutex mtx;
    std::vector<std::thread> threads;
    threads.reserve(concurentThreadsSupported);
    for (size_t i = 0; i < concurentThreadsSupported; ++i)
    {
        threads.emplace_back([this, cancel, &armoury, concurentThreadsSupported, i, &mtx]() {
            // concurentThreadsSupported = 1;
            // size_t i = 0;
            auto start = stats.possibleCombinations * i / concurentThreadsSupported;
            auto stop = stats.possibleCombinations * (i + 1) / concurentThreadsSupported - 1;
            util::GreyCodeGenerator gen({heads.size() - 1, torsos.size() - 1, arms.size() - 1,
                                         legs.size() - 1, weapons.size() - 1},
                                        start);
            std::vector<size_t> last = gen.currentGreyCode();
            Gear::ArmourSet set(armoury, heads[last[0]], torsos[last[1]], arms[last[2]],
                                legs[last[3]], weapons[last[4]]);
            auto increaseStats = [this]() {
                ++stats.searchedCombinations;
                int currentProgress = 100 * stats.searchedCombinations / stats.possibleCombinations;
                if (currentProgress != stats.progress)
                {
                    stats.progress = currentProgress;
                    stats.end = std::chrono::high_resolution_clock::now();
                    progress(stats);
                }
            };

            checkSet(set, armoury, availableCells);
            increaseStats();
            while (gen.generateNext() && *cancel != true && start < stop)
            {
                ++start;
                const auto& current = gen.currentGreyCode();
                if (current[0] != last[0])
                {
                    set.setHead(heads[current[0]]);
                }
                else if (current[1] != last[1])
                {
                    set.setTorso(torsos[current[1]]);
                }
                else if (current[2] != last[2])
                {
                    set.setArms(arms[current[2]]);
                }
                else if (current[3] != last[3])
                {
                    set.setLegs(legs[current[3]]);
                }
                else if (current[4] != last[4])
                {
                    set.setWeapon(weapons[current[4]]);
                }
                checkSet(set, armoury, availableCells);
                last = current;
                increaseStats();
            }
        });
    }
    for (auto& thread : threads)
    {
        thread.join();
    }

    stats.progress = 100;
    stats.end = std::chrono::high_resolution_clock::now();
    progress(stats);
    return;
}

void ArmourSetSearch::checkSet(Gear::ArmourSet& set, const Gear::Armoury& armoury,
                               const Gear::AvailableCellList& cells)
{
    std::array<size_t, Gear::SkillType::MaxSkillType> NeededCellsPerType = {};
    for (const auto& skillId : wantedSkills.getWantedSkills())
    {
        auto existingSkillPoints = set.getSkillPointsFor(skillId);
        if (existingSkillPoints >= wantedSkills.getWantedLevel(skillId))
            continue;
        auto remainingSkillPoints = wantedSkills.getWantedLevel(skillId) - existingSkillPoints;
        if (!cells.hasEnoughCellsFor(skillId, remainingSkillPoints))
            return;
        NeededCellsPerType[armoury.getSkillTypeFor(skillId)] +=
            cells.cellsNeededForSkillCount(skillId, remainingSkillPoints);
    }
    for (size_t i = Gear::SkillType::None + 1; i < Gear::SkillType::MaxSkillType; ++i)
    {
        if (NeededCellsPerType[i] > set.getCellSlotCountFor(Gear::SkillType(i)))
            return;
    }
    for (const auto& skillId : wantedSkills.getWantedSkills())
    {
        auto type = armoury.getSkillTypeFor(skillId);
        auto existingSkillPoints = set.getSkillPointsFor(skillId);
        if (existingSkillPoints >= wantedSkills.getWantedLevel(skillId))
            continue;
        auto remainingSkillPoints = wantedSkills.getWantedLevel(skillId) - existingSkillPoints;
        for (const auto& cellLevel :
             cells.getCellLevelsForSkillCount(skillId, remainingSkillPoints))
        {
            if (!set.addCell(Gear::Cell(Gear::Skill(skillId, cellLevel), type)))
            {
                // we should never land here
                std::cout << "added a cell which did not fit" << std::endl;
                set.removeAllCells();
                return;
            }
        }
    }
    ++stats.foundSets;
    addArmourSet(set);
    set.removeAllCells();
}

void ArmourSetSearch::addArmourSet(const Gear::ArmourSet& set)
{
    std::lock_guard lock(addMutex);
    armourSets.push_back(set);
}

const std::vector<Gear::ArmourSet>& ArmourSetSearch::getArmourSets() const { return armourSets; }

void ArmourSetSearch::setAvaiableCells(Gear::AvailableCellList&& availableCells)
{
    this->availableCells = std::move(availableCells);
}

void ArmourSetSearch::setProgressCallback(ProgressCallBack progress) { this->progress = progress; }

const Gear::WantedSkillList& ArmourSetSearch::getWantedSkills() const { return wantedSkills; }

std::vector<Gear::ArmourSet>&& ArmourSetSearch::moveArmourSets() { return std::move(armourSets); }