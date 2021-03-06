#ifndef DAUNTLESS_ASS_GEAR_GEAR_HPP
#define DAUNTLESS_ASS_GEAR_GEAR_HPP

#include "Dictionary.hpp"
#include "gear/Cell.hpp"
#include "gear/CellList.hpp"
#include "gear/Elements.hpp"
#include "gear/SkillList.hpp"
#include <optional>
#include <string>

namespace Gear
{
// contains all Information about a Gear
class GearInfo
{
  public:
    GearInfo(int id, const std::string& name, const std::string& description,
             const std::optional<Element>& elementalStrength = {},
             const std::optional<Element>& elementalWeakness = {});
    GearInfo(int id, std::string&& name, std::string&& description,
             std::optional<Element>&& elementalStrength = {},
             std::optional<Element>&& elementalWeakness = {});
    GearInfo(const GearInfo& other) = delete;
    GearInfo(GearInfo&& other) = delete;
    const GearInfo& operator=(const GearInfo& other) = delete;
    const GearInfo& operator=(GearInfo&& other) = delete;
    std::string name_;
    std::string description_;
    std::optional<Element> elementalStrength_, elementalWeakness_;
    int id_; // TODO like perks use this id as index instead of a shared pointer
};

class Gear
{
  public:
    Gear(std::shared_ptr<GearInfo>& info, int level, const Skill& skill,
         std::shared_ptr<std::vector<std::string>>& uniqueSkills, const std::vector<Cell>& cells);
    Gear(std::shared_ptr<GearInfo>&& info, int level, Skill&& skill,
         std::shared_ptr<std::vector<std::string>>&& uniqueSkills, std::vector<Cell>&& cells);

    Gear(const Gear& other) = default;
    Gear(Gear&& other) = default;
    const Gear& operator=(const Gear& other);
    const Gear& operator=(Gear&& other);

    // checks if the gear has free cell slots for a specific skill
    bool hasFreeCellSlotFor(SkillType type) const;
    // adds a new Cell to the gear
    // returns if the cell could be inserted
    bool addCell(const Cell& cell);

    /**
     * Removes all cells from the gear.
     */
    void removeAllCells();

    // returns the cells on the gear
    CellList getCellList() const;

    const std::string& getName() const;
    int Gear::Gear::getLevel() const;
    const std::string& Gear::Gear::getDescription() const;

    /**
     * check if the Gear has a specific skill
     * @param skillId The ID of the skill which shall be checked
     * @return Returns true if the gear has the specified skill, otherwise falls
     */
    bool hasSkill(size_t skillId) const;
    /**
     * check the skill points for a specific skill with cells
     * @param skillId The ID of the skill which shall be checked
     * @return Returns the skill points for the specified skill
     */
    size_t getSkillPointsFor(size_t skillId) const;

    /**
     * gathers all innate skill of the gear. This does not include skills from cells
     * @return Returns a list of skills
     */
    const Skill& getInnateSkills() const;
    /**
     * gathers all skills on the gear including the ones gained through cells
     * @return Returns a list of skills
     */
    SkillList getSkills() const;
    // returns a list of all unique skills on the gear
    const std::vector<std::string>& getUniqueSkills() const;
    bool hasUniqueSkill() const;
    // returns the info of the gear as human readable string
    std::string getGearInfo(const Dictionary& dict) const;

    /**
     * @return Returns the elemental strength of the qear if available
     */
    const std::optional<Element>& getElementalStrength() const;

    /**
     * @return Returns the elemental weakness of the qear if available
     */
    const std::optional<Element>& getElementalWeakness() const;

    virtual std::string getToolTip(const Dictionary& dict, const Armoury& armoury) const;

    int getId() const;

  private:
    std::shared_ptr<GearInfo> info_;
    int level_;
    Skill skill_;
    std::shared_ptr<std::vector<std::string>> uniqueSkills_;
    std::vector<Cell> cells_;
};
} // namespace Gear

#endif // ! DAUNTLESS_ASS_GEAR_GEAR_HPP
