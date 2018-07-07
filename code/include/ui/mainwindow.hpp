#ifndef DAUNTLESS_ASS_UI_MAINWINDOW_HPP
#define DAUNTLESS_ASS_UI_MAINWINDOW_HPP

#include "ArmourSetSearch.hpp"
#include "Dictionary.hpp"
#include "Options.hpp"
#include "gear/Armoury.hpp"
#include "ui/SkillSelector.hpp"
#include <QComboBox>
#include <QMainWindow>
#include <unordered_map>

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

  private:
    void setupTranslation();
    std::vector<Gear::Skill> getWantedSkills();
    void search();
    void advancedSearch();
    void setSearchButtonsState(bool enable);
    void armourSetSearch(ArmourSetSearch &ass);
    void setNumberOfResults(QAction *action);
    void setLanguage(QAction *action);
    void about();
    void showArmourSets(const std::vector<Gear::ArmourSet> &armoursets);

        Options options;
    std::vector<SkillSelector *> skillSelectors;
    Ui::MainWindow *ui;
    Dictionary dict;
    Gear::Armoury armoury;
    std::unordered_map<Gear::SkillType, std::vector<const Gear::SkillInfo *>> skills;
};

#endif // !DAUNTLESS_ASS_UI_MAINWINDOW_HPP
