#ifndef DAUNTLESS_ASS_UI_MAINWINDOW_HPP
#define DAUNTLESS_ASS_UI_MAINWINDOW_HPP

#include "ArmourSetSearch.hpp"
#include "Dictionary.hpp"
#include "Options.hpp"
#include "gear/Armoury.hpp"
#include "ui/SkillSelector.hpp"
#include <QComboBox>
#include <QFutureWatcher>
#include <QMainWindow>
#include <set>
#include <unordered_map>
#include <QLabel>

namespace Ui
{
class MainWindow;
}

class QNetworkReply;
class QNetworkAccessManager;
class QListWidgetItem;
class ArmourSetView;

class Filter
{
  public:
    std::set<std::string> weapons;
    std::set<Gear::SkillType> cellSlots;
    std::set<Gear::Skill> additionalSkills;
    void clear();
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void closeEvent(QCloseEvent *event) override;

  private slots:
    void setProgress(ArmourSetSearch::SearchStatistics stats);
    void finishedSearch(ArmourSetSearch *ass);
  signals:
    void setProgressMainThread(ArmourSetSearch::SearchStatistics stats);
    void finishedSearchMainThread(ArmourSetSearch *ass);

  private:
    void setupTranslation();
    std::vector<Gear::Skill> getWantedSkills();
    void search();
    void advancedSearch();
    void setSearchButtonsState(bool enable);
    void armourSetSearch(ArmourSetSearch *ass);
    void setNumberOfResults(QAction *action);
    void setLanguage(QAction *action);
    void about();
    void showArmourSets();
    ArmourSetView* createArmourSetView(const Gear::ArmourSet& set, std::vector<Gear::Skill> wantedSkills);
    void createArmourSetItem(const Gear::ArmourSet *set, ArmourSetView *view);
    void showLoadedSearch();
    void saveSearchSettings();
    void saveSearch();
    void loadSearch();
    void updateNetworkReply(QNetworkReply *);
    void clearSearch();
    void applyFilter();
    void saveDataFiles(QNetworkReply *reply, const QString &fileName);

    Options options;
    std::vector<SkillSelector *> skillSelectors;
    Ui::MainWindow *ui;
    Dictionary dict;
    Gear::Armoury armoury;
    std::unordered_map<Gear::SkillType, std::vector<const Gear::SkillInfo *>> skills;
    QNetworkAccessManager *manager = NULL;
    bool cancelSearch = false;
    QFutureWatcher<void> *searchWatcher = new QFutureWatcher<void>();
    Filter filter;
    bool isCreatingArmourSets = false;
    std::unordered_map<const Gear::ArmourSet *, QListWidgetItem *> armourSetItems;
    int armourSetViewGearWidth = 0;
    int armourSetViewCellWidth = 0;
    QLabel* statCombinations;
    QLabel* statFoundSets;
    QLabel* statElapsedTime;
    QLabel* statSearchesPerSecond;
};

#endif // !DAUNTLESS_ASS_UI_MAINWINDOW_HPP
