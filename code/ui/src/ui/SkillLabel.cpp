#include "ui/SkillLabel.hpp"
#include <sstream>
#include "util/Translation.hpp"

SkillLabel::SkillLabel(const Gear::Skill& skill, const Gear::Armoury& armoury,
                       const Dictionary& dict, QWidget* parent)
    : QLabel(parent)
{
    setText(QString::fromStdString(skill.toString(dict, armoury)));
    const auto& skillInfo = armoury.getSkillInfo(skill.getId());
    std::stringstream stream;
    stream << "<p style='white-space:pre'>";
    for (size_t i = 1; i <= skillInfo.getMaxLevel(); ++i) 
    {
        if (i == skill.getSkillPoints())
            stream << "<b>";
        stream << skillInfo.getEffectForLevel(i);
        if (i == skill.getSkillPoints())
            stream << "</b>";
        if (i != skillInfo.getMaxLevel())
            stream << "<br>";
    }
    setToolTip(QString::fromStdString(stream.str()));
}
