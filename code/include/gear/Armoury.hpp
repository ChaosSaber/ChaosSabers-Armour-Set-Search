#ifndef DAUNTLESS_ASS_GEAR_ARMOURY_HPP
#define DAUNTLESS_ASS_GEAR_ARMOURY_HPP

#include "gear/Armour.hpp"
#include "gear/SkillInfo.hpp"
#include "gear/Weapon.hpp"
#include <QJsonObject>
#include <vector>

namespace Gear
{
class Armoury
{
  public:
    Armoury();

    const SkillInfo &getSkillInfoFor(const std::string &name) const;
    std::vector<Weapon> getWeaponsWithSkill(const std::vector<Skill> &skills,
                                            WeaponType type) const;
    std::vector<Armour> getArmourWithSkill(const std::vector<Skill> &skills, ArmourType type) const;
    const Armour &getArmour(std::string name) const;
    const Weapon &getWeapon(std::string name) const;

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

    //Skill getSkill(const std::string &name, const std::string &points);
    //std::vector<Weapon> loadWeapons(const std::string &fileName, WeaponType type);
    //std::vector<Armour> loadArmour(const std::string &fileName, ArmourType type);
    //std::vector<SkillInfo> loadSkillInfos(const std::string &fileName);

    std::unordered_map<WeaponType, std::vector<Weapon>> weapons;
    std::unordered_map<ArmourType, std::vector<Armour>> armours;
    std::vector<SkillInfo> skillInfos;
    SkillInfo notFound;
};
} // namespace Gear

#endif // !DAUNTLESS_ASS_GEAR_ARMOURY_HPP
