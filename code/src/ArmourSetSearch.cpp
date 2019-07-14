#include "ArmourSetSearch.hpp"
#include <sstream>
#include <iostream>
#include <thread>

ArmourSetSearch::ArmourSetSearch(const Gear::Armoury& armoury, Gear::WeaponType weaponType,
                                 std::vector<Gear::Skill> skills, const Options& options,
                                 ProgressCallBack progress,
                                 Gear::CellList availableCells)
    : ArmourSetSearch(armoury.getWeaponsWithSkill(skills, weaponType, options),
                      armoury.getArmourWithSkill(skills, Gear::Head, options),
                      armoury.getArmourWithSkill(skills, Gear::Torso, options),
                      armoury.getArmourWithSkill(skills, Gear::Arms, options),
                      armoury.getArmourWithSkill(skills, Gear::Legs, options), skills, progress,
                      availableCells)
{
}

ArmourSetSearch::ArmourSetSearch(std::vector<Gear::Weapon> weapons, std::vector<Gear::Armour> heads,
                                 std::vector<Gear::Armour> torsos, std::vector<Gear::Armour> arms,
                                 std::vector<Gear::Armour> legs,
                                 std::vector<Gear::Skill> wantedSkills, ProgressCallBack progress,
                                 Gear::CellList availableCells)
    : weapons(std::move(weapons)), heads(std::move(heads)), torsos(std::move(torsos)),
      arms(std::move(arms)), legs(std::move(legs)), wantedSkills(std::move(wantedSkills)),
      progress(progress),
      availableCells(std::move(availableCells))
{
    if (heads.empty())
        heads.push_back(Gear::Armour(Gear::ArmourType::Head, "any_hat", "any_hat", 0,
                                     Gear::Elements(), std::vector<std::string>(),
                                     Gear::SkillType::None));
    if (torsos.empty())
        torsos.push_back(Gear::Armour(Gear::ArmourType::Torso, "any_torso", "any_torso", 0,
                                      Gear::Elements(), std::vector<std::string>(),
                                      Gear::SkillType::None));
    if (arms.empty())
        arms.push_back(Gear::Armour(Gear::ArmourType::Arms, "any_arms", "any_arms", 0,
                                    Gear::Elements(), std::vector<std::string>(),
                                    Gear::SkillType::None));
    if (legs.empty())
        legs.push_back(Gear::Armour(Gear::ArmourType::Legs, "any_legs", "any_legs", 0,
                                    Gear::Elements(), std::vector<std::string>(),
                                    Gear::SkillType::None));
    if (weapons.empty())
        weapons.push_back(Gear::Weapon(Gear::WeaponType::Sword, "any_hat", "any_hat", 0,
                                       Gear::Elements(), std::vector<std::string>(),
                                       Gear::SkillType::None, Gear::SkillType::None));
}

void ArmourSetSearch::search(const Gear::Armoury& armoury, const bool* cancel)
{
    // may return 0 when not able to detect
    auto concurentThreadsSupported = std::thread::hardware_concurrency();
    if (concurentThreadsSupported == 0)
        concurentThreadsSupported = 1;
    std::cout << "searching armoursets with " << concurentThreadsSupported << " threads"
              << std::endl;
    // TODO: maximum number of found sets to prevent out of memory
    reset();
    stats.possibleCombinations =
        heads.size() * torsos.size() * arms.size() * legs.size() * weapons.size();
    stats.searchedCombinations = 0;
    stats.foundSets = 0;
    stats.progress = 0;
    stats.start = std::chrono::high_resolution_clock::now();
    stats.end = std::chrono::high_resolution_clock::now();
    progress(stats);
    std::vector<std::thread> threads;
    threads.reserve(concurentThreadsSupported);
    for (size_t i = 0; i < concurentThreadsSupported; ++i)
    {
        threads.emplace_back([this, cancel,&armoury, concurentThreadsSupported]() {
            // This is more or less an arbitrary value.
            // it was waiting too long on the mutex when it got one set at a time. so i increased the amount it got at once
            // 200 felt right. For a better value a proper Benchmark is necesary
            constexpr size_t setCount = 200;
            std::vector<Gear::ArmourSet> sets;
            sets.reserve(setCount);
            while (!(*cancel) && getNextArmourSets(setCount, sets))
            {
                for (auto&& set:sets)
                    checkSet(std::move(set), armoury);
            }
        });
    }
    for (auto& thread : threads)
    {
        thread.join();
    }

    stats.progress = 100 * stats.searchedCombinations / stats.possibleCombinations;
    stats.end = std::chrono::high_resolution_clock::now();
    progress(stats);
    return;
}

void ArmourSetSearch::checkSet(Gear::ArmourSet set, const Gear::Armoury& armoury)
{
    Gear::CellList cells = availableCells;
    for (const auto& skill : wantedSkills)
    {
        int existingSkillPoints = set.getSkillPointsFor(skill.getName());
        if (!cells.hasEnoughCellsFor(skill, existingSkillPoints))
            return;
        auto type = armoury.getSkillInfoFor(skill.getName()).getType();
        while (existingSkillPoints < skill.getSkillPoints())
        {
            int bestCellLevel = cells.getOptimalCellLevel(skill, existingSkillPoints);
            Gear::Cell cell(Gear::Skill(skill.getName(), bestCellLevel), type);
            if (!set.addCell(cell))
                return;
            cells -= cell;
            existingSkillPoints += bestCellLevel;
        }
    }
    addArmourSet(std::move(set));
}

void ArmourSetSearch::addArmourSet(Gear::ArmourSet&& set)
{
    std::lock_guard lock(addMutex);
    armourSets.push_back(set);
}

bool ArmourSetSearch::getNextArmourSets(size_t count, std::vector<Gear::ArmourSet>& sets)
{
    sets.clear();
    std::lock_guard lock(getArmourSetMutex);
    for (size_t i = 0; i < count; ++i)
    {
        if (indexHead >= heads.size())
            break;
        sets.emplace_back(heads[indexHead], torsos[indexTorso], arms[indexArm], legs[indexLeg],
                          weapons[indexWeapon]);
        ++indexWeapon;
        if (indexWeapon >= weapons.size())
        {
            indexWeapon = 0;
            ++indexLeg;
            if (indexLeg >= legs.size())
            {
                indexLeg = 0;
                ++indexArm;
                if (indexArm >= arms.size())
                {
                    indexArm = 0;
                    ++indexTorso;
                    if (indexTorso >= torsos.size())
                    {
                        indexTorso = 0;
                        ++indexHead;
                    }
                }
            }
        }
    }
    if (sets.size() == 0)
        return false;
    stats.searchedCombinations += sets.size();
    int currentProgress = 100 * stats.searchedCombinations / stats.possibleCombinations;
    if (currentProgress != stats.progress)
    {
        stats.progress = currentProgress;
        stats.end = std::chrono::high_resolution_clock::now();
        progress(stats);
    }
    return true;
}

void ArmourSetSearch::reset() { indexWeapon = indexHead = indexTorso = indexArm = indexLeg = 0; }

const std::vector<Gear::ArmourSet>& ArmourSetSearch::getArmourSets() const { return armourSets; }

void ArmourSetSearch::setAvaiableCells(Gear::CellList availableCells)
{
    this->availableCells = std::move(availableCells);
}

void ArmourSetSearch::setProgressCallback(ProgressCallBack progress) { this->progress = progress; }

const std::vector<Gear::Skill>& ArmourSetSearch::getWantedSkills() const { return wantedSkills; }

std::vector<Gear::ArmourSet>&& ArmourSetSearch::moveArmourSets() { return std::move(armourSets); }