#ifndef DAUNTLESS_ASS_GEAR_ARMOURY_HPP
#define DAUNTLESS_ASS_GEAR_ARMOURY_HPP

#include "Dictionary.hpp"
#include "gear/Armour.hpp"
#include "gear/SkillInfo.hpp"
#include "gear/Weapon.hpp"
#include <QJsonObject>
#include <vector>

class Options;

namespace Gear
{
class Armoury
{
  public:
    Armoury(Dictionary &dict);

    void loadData();

    const SkillInfo &getSkillInfoFor(const std::string &name) const;
    std::vector<Weapon> getWeaponsWithSkill(const std::vector<Skill> &skills, WeaponType type,
                                            const Options &options) const;
    std::vector<Armour> getArmourWithSkill(const std::vector<Skill> &skills, ArmourType type,
                                           const Options &options) const;
    const Armour &getArmour(std::string name, bool heroic) const;
    const Weapon &getWeapon(std::string name, bool heroic) const;

    // returns the skill info to all skills of the specified type
    // filters out unique skills
    std::vector<const SkillInfo *> getSkills(SkillType filter) const;
    SkillType getSkillTypeFor(const std::string &skillName) const;

  private:
    void load(const std::string &fileName = "data/data.json");
    void setElement(Elements &element, const std::string &name, int value);
    SkillType getSkillType(const std::string &type) const;
    ArmourType getArmourType(const std::string &type) const;
    WeaponType getWeaponType(const std::string &type) const;
    Rarity getRarity(const QJsonObject &gear) const;
    std::vector<int> getTiers(const QJsonObject &gear) const;

    /**
    * looks up if the found gear is the correct one
    * @param gear the gear to check
    * @param heroic ???
    * @return returns if it is the correct gear
    */
    bool isCorrectGear(const Gear &gear, bool heroic) const;

    // filters out gear according to its tier
    // its filtered out if the tier is higher than the current progression and if its tier is too low and don't use lower tier gear is enabled
    // returns true if its filtered out
    bool filterGear(const Gear &gear, const Options &options) const;
    bool isLowerTier(int gearTier, const Options &options) const;


    std::unordered_map<WeaponType, std::vector<Weapon>> weapons;
    std::unordered_map<ArmourType, std::vector<Armour>> armours;
    std::vector<SkillInfo> skillInfos;
    SkillInfo notFound;
    Dictionary &dict;
};
} // namespace Gear

#endif // !DAUNTLESS_ASS_GEAR_ARMOURY_HPP
