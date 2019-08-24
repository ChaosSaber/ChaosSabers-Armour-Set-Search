#include "ArmourSetSearch.hpp"
#include "util/GreyCodeGenerator.hpp"
#include <iostream>
#include <sstream>
#include <thread>

ArmourSetSearch::ArmourSetSearch(const Gear::Armoury& armoury, Gear::WeaponType weaponType,
                                 std::vector<Gear::Skill> skills, const Options& options,
                                 ProgressCallBack progress, Gear::CellList availableCells)
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
      progress(progress), availableCells(std::move(availableCells))
{
    if (heads.empty())
        heads.push_back(
            Gear::Armour(Gear::ArmourType::Head,
                         std::make_shared<Gear::GearInfo>("any_hat", "any_hat", Gear::Elements()),
                         0, std::make_shared<std::vector<std::string>>(), Gear::SkillType::None));
    if (torsos.empty())
        torsos.push_back(Gear::Armour(
            Gear::ArmourType::Torso,
            std::make_shared<Gear::GearInfo>("any_torso", "any_torso", Gear::Elements()), 0,
            std::make_shared<std::vector<std::string>>(), Gear::SkillType::None));
    if (arms.empty())
        arms.push_back(
            Gear::Armour(Gear::ArmourType::Arms,
                         std::make_shared<Gear::GearInfo>("any_arms", "any_arms", Gear::Elements()),
                         0, std::make_shared<std::vector<std::string>>(), Gear::SkillType::None));
    if (legs.empty())
        legs.push_back(
            Gear::Armour(Gear::ArmourType::Legs,
                         std::make_shared<Gear::GearInfo>("any_legs", "any_legs", Gear::Elements()),
                         0, std::make_shared<std::vector<std::string>>(), Gear::SkillType::None));
    if (weapons.empty())
        weapons.push_back(Gear::Weapon(
            Gear::WeaponType::Sword,
            std::make_shared<Gear::GearInfo>("any_weapon", "any_weapon", Gear::Elements()), 0,
            std::make_shared<std::vector<std::string>>(), Gear::SkillType::None,
            Gear::SkillType::None));
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
    std::mutex mtx;
    std::vector<std::thread> threads;
    threads.reserve(concurentThreadsSupported);
    for (size_t i = 0; i < concurentThreadsSupported; ++i)
    {
        threads.emplace_back([this, cancel, &armoury, concurentThreadsSupported, i, &mtx]() {
            auto start = stats.possibleCombinations * i / concurentThreadsSupported;
            auto stop = stats.possibleCombinations * (i + 1) / concurentThreadsSupported - 1;
            util::GreyCodeGenerator gen({heads.size() - 1, torsos.size() - 1, arms.size() - 1,
                                         legs.size() - 1, weapons.size() - 1},
                                        start);
            std::vector<size_t> last = gen.currentGreyCode();
            Gear::ArmourSet set(heads[last[0]], torsos[last[1]], arms[last[2]], legs[last[3]],
                                weapons[last[4]]);
            Gear::CellList cells = availableCells;
            checkSet(set, armoury, cells);
            auto returnCells = [&cells](Gear::CellList cellList) {
                for (const auto& cell : cellList)
                    if (!cell.first.isEmpty())
                        cells += cell;
            };
            while (gen.generateNext() && *cancel != true && start < stop)
            {
                ++start;
                const auto& current = gen.currentGreyCode();
                if (current[0] != last[0])
                {
                    returnCells(set.getHead().getCellList());
                    set.setHead(heads[current[0]]);
                    for (auto& skill : set.getHead().getInnateSkills())
                        returnCells(set.removeCells(skill));
                }
                else if (current[1] != last[1])
                {
                    returnCells(set.getTorso().getCellList());
                    set.setTorso(torsos[current[1]]);
                    for (auto& skill : set.getTorso().getInnateSkills())
                        returnCells(set.removeCells(skill));
                }
                else if (current[2] != last[2])
                {
                    returnCells(set.getArms().getCellList());
                    set.setArms(arms[current[2]]);
                    for (auto& skill : set.getArms().getInnateSkills())
                        returnCells(set.removeCells(skill));
                }
                else if (current[3] != last[3])
                {
                    returnCells(set.getLegs().getCellList());
                    set.setLegs(legs[current[3]]);
                    for (auto& skill : set.getLegs().getInnateSkills())
                        returnCells(set.removeCells(skill));
                }
                else if (current[4] != last[4])
                {
                    returnCells(set.getWeapon().getCellList());
                    set.setWeapon(weapons[current[4]]);
                    for (auto& skill : set.getWeapon().getInnateSkills())
                        returnCells(set.removeCells(skill));
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
                               Gear::CellList& cells)
{
    for (const auto& skill : wantedSkills)
    {
        auto existingSkillPoints = set.getSkillPointsFor(skill.getId());
        if (!cells.hasEnoughCellsFor(skill, existingSkillPoints))
            return;
        auto type = armoury.getSkillTypeFor(skill.getId());
        while (existingSkillPoints < skill.getSkillPoints())
        {
            auto highestCellLevel = cells.getHighestAvailableCellLevel(skill);
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

void ArmourSetSearch::reset() { indexWeapon = indexHead = indexTorso = indexArm = indexLeg = 0; }

const std::vector<Gear::ArmourSet>& ArmourSetSearch::getArmourSets() const { return armourSets; }

void ArmourSetSearch::setAvaiableCells(Gear::CellList availableCells)
{
    this->availableCells = std::move(availableCells);
}

void ArmourSetSearch::setProgressCallback(ProgressCallBack progress) { this->progress = progress; }

const std::vector<Gear::Skill>& ArmourSetSearch::getWantedSkills() const { return wantedSkills; }

std::vector<Gear::ArmourSet>&& ArmourSetSearch::moveArmourSets() { return std::move(armourSets); }