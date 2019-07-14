#include "ArmourSetSearch.hpp"
#include <sstream>

ArmourSetSearch::ArmourSetSearch(const Gear::Armoury& armoury, Gear::WeaponType weaponType,
                                 std::vector<Gear::Skill> skills, const Options& options,
                                 Gear::CellList availableCells)
    : ArmourSetSearch(armoury.getWeaponsWithSkill(skills, weaponType, options),
                      armoury.getArmourWithSkill(skills, Gear::Head, options),
                      armoury.getArmourWithSkill(skills, Gear::Torso, options),
                      armoury.getArmourWithSkill(skills, Gear::Arms, options),
                      armoury.getArmourWithSkill(skills, Gear::Legs, options), skills,
                      availableCells)
{
}

ArmourSetSearch::ArmourSetSearch(std::vector<Gear::Weapon> weapons, std::vector<Gear::Armour> heads,
                                 std::vector<Gear::Armour> torsos, std::vector<Gear::Armour> arms,
                                 std::vector<Gear::Armour> legs,
                                 std::vector<Gear::Skill> wantedSkills,
                                 Gear::CellList availableCells)
    : weapons(std::move(weapons)), heads(std::move(heads)), torsos(std::move(torsos)),
      arms(std::move(arms)), legs(std::move(legs)), wantedSkills(std::move(wantedSkills)),
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

void ArmourSetSearch::search(const Gear::Armoury& armoury, bool* cancel,
                             std::function<void(SearchStatistics)> progress)
{
    // TODO: maximum number of found sets to prevent out of memory
    stats.possibleCombinations =
        heads.size() * torsos.size() * arms.size() * legs.size() * weapons.size();
    stats.searchedCombinations = 0;
    stats.foundSets = 0;
    stats.progress = 0;
    stats.start = std::chrono::high_resolution_clock::now();
    stats.end = std::chrono::high_resolution_clock::now();
    progress(stats);
    for (const auto& head : heads)
        for (const auto& torso : torsos)
            for (const auto& arm : arms)
                for (const auto& leg : legs)
                    for (const auto& weapon : weapons)
                    {
                        if (*cancel)
                            break;
                        checkSet(Gear::ArmourSet(head, torso, arm, leg, weapon), armoury);
                        ++stats.searchedCombinations;
                        int currentProgress =
                            100 * stats.searchedCombinations / stats.possibleCombinations;
                        if (currentProgress != stats.progress)
                        {
                            stats.progress = currentProgress;
                            stats.end = std::chrono::high_resolution_clock::now();
                            progress(stats);
                        }
                    }
    if (stats.searchedCombinations != stats.possibleCombinations)
    {
        stats.progress = 100 * stats.searchedCombinations / stats.possibleCombinations;
        stats.end = std::chrono::high_resolution_clock::now();
    }
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
    armourSets.push_back(set);
}

std::string ArmourSetSearch::toString(const Dictionary& dict)
{
    std::stringstream ss;
    for (const auto& armourSet : armourSets)
        ss << armourSet.getGearInfo(dict) << std::endl << std::string(70, '-') << std::endl;
    return ss.str();
}

const std::vector<Gear::ArmourSet>& ArmourSetSearch::getArmourSets() const { return armourSets; }

void ArmourSetSearch::setAvaiableCells(Gear::CellList availableCells)
{
    this->availableCells = std::move(availableCells);
}

const std::vector<Gear::Skill>& ArmourSetSearch::getWantedSkills() const { return wantedSkills; }

std::vector<Gear::ArmourSet>&& ArmourSetSearch::moveArmourSets() { return std::move(armourSets); }