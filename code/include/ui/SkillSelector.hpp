#ifndef DAUNTLESS_ASS_UI_SKILLSELECTOR_HPP
#define DAUNTLESS_ASS_UI_SKILLSELECTOR_HPP

#include "Dictionary.hpp"
#include "Options.hpp"
#include "gear/SKillType.hpp"
#include "gear/Skill.hpp"
#include "gear/SkillInfo.hpp"
#include <QComboBox>
#include <QGridLayout>
#include <QSpinBox>
#include <unordered_map>

class SkillSelector : public QObject
{
    Q_OBJECT

  public:
    explicit SkillSelector(
        const Dictionary &dict,
        std::unordered_map<Gear::SkillType, std::vector<const Gear::SkillInfo *>> &skills,
        QGridLayout *layout);

    void getSkill(std::vector<Gear::Skill> &skills) const;
    void set(const Options::SkillSearch &search);
    Options::SkillSearch getSearchSettings() const;
    void clear();

  private:
    std::string getSkillName() const;
    void setSkillTypeFilter();
    void filterChanged(Gear::SkillType filter);

    QComboBox *skillType, *skillname;
    QSpinBox *skillValue;
    const Dictionary &dict;
    std::unordered_map<Gear::SkillType, std::vector<const Gear::SkillInfo *>> &skills;
};

#endif // !DAUNTLESS_ASS_UI_SKILLSELECTOR_HPP