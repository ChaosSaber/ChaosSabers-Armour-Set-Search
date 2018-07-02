#include "ArmourSetSearch.hpp"
#include "ui/mainwindow.hpp"
#include <QApplication>
#include <iostream>

void addSkillsAndCells(std::vector<Gear::Skill> &skills, Gear::CellList &cells,
                       const std::string &skillName, int skillLevel, Gear::SkillType type,
                       int cellLevel3, int cellLevel2, int cellLevel1)
{
    skills.push_back({skillName, skillLevel});
    cells += Gear::Cell(Gear::Skill(skillName, 3), type) * cellLevel3;
    cells += Gear::Cell(Gear::Skill(skillName, 2), type) * cellLevel2;
    cells += Gear::Cell(Gear::Skill(skillName, 1), type) * cellLevel1;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
