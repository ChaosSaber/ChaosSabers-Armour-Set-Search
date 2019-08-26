#include "ArmourSetSearch.hpp"
#include "util/GreyCodeGenerator.hpp"
#include <iostream>
#include <sstream>
#include <thread>

ArmourSetSearch::ArmourSetSearch(const Gear::Armoury& armoury, Gear::WeaponType weaponType,
                                 std::vector<Gear::Skill> skills, const Options& options)
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
                                 std::vector<Gear::Skill> wantedSkills)
    : weapons(std::move(weapons)), heads(std::move(heads)), torsos(std::move(torsos)),
      arms(std::move(arms)), legs(std::move(legs)), wantedSkills(std::move(wantedSkills))
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
    //concurentThreadsSupported = 1;
    //size_t i = 0;
            auto start = stats.possibleCombinations * i / concurentThreadsSupported;
            auto stop = stats.possibleCombinations * (i + 1) / concurentThreadsSupported - 1;
            util::GreyCodeGenerator gen({heads.size() - 1, torsos.size() - 1, arms.size() - 1,
                                         legs.size() - 1, weapons.size() - 1},
                                        start);
            std::vector<size_t> last = gen.currentGreyCode();
            Gear::ArmourSet set(heads[last[0]], torsos[last[1]], arms[last[2]], legs[last[3]],
                                weapons[last[4]]);
            Gear::CellList2 cells = availableCells;
            checkSet(set, armoury, cells);
            auto switchGear = [&cells](Gear::ArmourSet& set, const Gear::Gear& oldGear, const Gear::Gear& newGear) {
                cells += oldGear.getCellList();
                for (auto& skill : newGear.getInnateSkills())
                    cells += set.removeCells(skill);
            };
            while (gen.generateNext() && *cancel != true && start < stop)
            {
                ++start;
                const auto& current = gen.currentGreyCode();
                if (current[0] != last[0])
                {
                    switchGear(set, set.getHead(), heads[current[0]]);
                    set.setHead(heads[current[0]]);
                    
                }
                else if (current[1] != last[1])
                {
                    switchGear(set, set.getTorso(), torsos[current[1]]);
                    set.setTorso(torsos[current[1]]);
                }
                else if (current[2] != last[2])
                {
                    switchGear(set, set.getArms(), arms[current[2]]);
                    set.setArms(arms[current[2]]);
                }
                else if (current[3] != last[3])
                {
                    switchGear(set, set.getLegs(), legs[current[3]]);
                    set.setLegs(legs[current[3]]);
                }
                else if (current[4] != last[4])
                {
                    switchGear(set, set.getWeapon(), weapons[current[4]]);
                    set.setWeapon(weapons[current[4]]);
                }
                checkSet(set, armoury, cells);
                last = current;

                ++stats.searchedCombinations;
                {
                    std::lock_guard lock(mtx);
                    int currentProgress =
                        100 * stats.searchedCombinations / stats.possibleCombinations;
                    if (currentProgress != stats.progress)
                    {
                        stats.progress = currentProgress;
                        stats.end = std::chrono::high_resolution_clock::now();
                        progress(stats);
                    }
                }
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
                               Gear::CellList2& cells)
{
    for (const auto& skill : wantedSkills)
    {
        auto existingSkillPoints = set.getSkillPointsFor(skill.getId());
        if (existingSkillPoints>=skill.getSkillPoints())
            continue;
        if (!cells.hasEnoughCellsFor(skill.getId(), skill.getSkillPoints() - existingSkillPoints))
            return;
        auto type = armoury.getSkillTypeFor(skill.getId());
        while (existingSkillPoints < skill.getSkillPoints())
        {
            auto highestCellLevel = cells.getHighestAvailableCellLevel(skill.getId());
            if (highestCellLevel == 0) // no cell available
                return;
            Gear::Cell cell(Gear::Skill(skill.getId(), highestCellLevel), type);
            if (!set.addCell(cell))
                return;
            cells -= cell;
            existingSkillPoints += highestCellLevel;
        }
    }
    ++stats.foundSets;
    addArmourSet(set);
}

void ArmourSetSearch::addArmourSet(const Gear::ArmourSet& set)
{
    std::lock_guard lock(addMutex);
    armourSets.push_back(set);
}

const std::vector<Gear::ArmourSet>& ArmourSetSearch::getArmourSets() const { return armourSets; }

void ArmourSetSearch::setAvaiableCells(Gear::CellList2&& availableCells)
{
    this->availableCells = std::move(availableCells);
}

void ArmourSetSearch::setProgressCallback(ProgressCallBack progress) { this->progress = progress; }

const std::vector<Gear::Skill>& ArmourSetSearch::getWantedSkills() const { return wantedSkills; }

std::vector<Gear::ArmourSet>&& ArmourSetSearch::moveArmourSets() { return std::move(armourSets); }