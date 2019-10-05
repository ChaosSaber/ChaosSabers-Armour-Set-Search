#pragma once

#include <QLabel>
#include <gear/Armoury.hpp>

class SkillLabel : public QLabel
{
  public:
    SkillLabel(const Gear::Skill& skill, const Gear::Armoury& armoury, const Dictionary& dict,
               QWidget* parent = NULL);
};