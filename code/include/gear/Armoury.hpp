#ifndef DAUNTLESS_ASS_GEAR_ARMOURY_HPP
#define DAUNTLESS_ASS_GEAR_ARMOURY_HPP

#include "Dictionary.hpp"
#include "gear/Armour.hpp"
#include "gear/SkillInfo.hpp"
#include "gear/Weapon.hpp"
#include <QJsonObject>
#include <string>
#include <vector>

class Options;

namespace Gear
{
class Armoury
{
  public:
    Armoury(Dictionary& dict);

    void loadData();

    std::vector<Weapon> getWeaponsWithSkill(const WantedSkillList& skills, WeaponType type,
                                            const Options& options) const;
    std::vector<Armour> getArmourWithSkill(const WantedSkillList& skills, ArmourType type,
                                           const Options& options) const;
    const Armour& getArmour(std::string name, int level) const;
    const Weapon& getWeapon(std::string name, int level) const;

    // returns the skill info to all skills of the specified type
    // filters out unique skills
    std::vector<const SkillInfo*> getSkills(SkillType filter) const;

    /**
     * Gets all skill infos inclusive the default empty skill(position 0)
     * @return Returns a vector with infos to all skills.
     */
    const std::vector<SkillInfo>& getSkillInfos() const;

    /**
     * @param id The ID of the skill the skill info is needed
     * @return Returns the SkillInfo for the specified ID
     */
    const SkillInfo& getSkillInfo(size_t id) const;
    /**
     * @param id The id of the skill from which the skilltype is needed
     * @return Returns the skill type if a skill with that ID exists, otherwise it will return
     * SkillType::None
     */
    SkillType getSkillTypeFor(size_t id) const;
    /**
     * searchs for the skill ID with the corresponding name
     * @param name Name of the skill
     * @return Returns the ID of the skill if found otherwise will throw
     */
    size_t getSkillIdForName(const std::string& name) const;

  private:
    void load(const std::string& fileName = "data/data.json");
    void setElement(Elements& element, const std::string& name, int value);
    SkillType getSkillType(const std::string& type) const;
    ArmourType getArmourType(const std::string& type) const;
    WeaponType getWeaponType(const std::string& type) const;

    // filters out gear according to its tier
    // its filtered out if the tier is higher than the current progression and if its tier is too
    // low and don't use lower tier gear is enabled returns true if its filtered out
    bool filterGear(const Gear& gear, const Options& options) const;

    /**
    * Checks if the gear has a needed skill or cell slot.
    * @param gear The gear to check.
    * @param skills A list of wanted skills.
    * @return Returns true if the gear has the needed requirements, otherwise false.
    */
    bool gearHasSkill(const Gear& gear, const WantedSkillList& skills) const;

    /**
     * Filters out weapons according to its tier and weapon element.
     * It's filtered out if the tier is higher than the current progression and if its tier is too
     * low and don't use lower tier gear is enabled. It's also filtered out if the weapon element is
     * not compatible.
     * @parameter weapon The weapon to check.
     * @parameter options The search options.
     * @return Returns true if the weapon should be filtered and false if not.
     */
    bool filterWeapon(const Weapon& weapon, const Options& options) const;

    std::unordered_map<WeaponType, std::vector<Weapon>> weapons;
    std::unordered_map<ArmourType, std::vector<Armour>> armours;

    std::vector<SkillInfo> skillInfos;
    std::unordered_map<std::string, size_t> mapSkillNameToId_;
    SkillInfo notFound;

    Dictionary& dict;
};
} // namespace Gear

#endif // !DAUNTLESS_ASS_GEAR_ARMOURY_HPP
