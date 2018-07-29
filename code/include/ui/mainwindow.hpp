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

namespace Ui
{
class MainWindow;
}

class QNetworkReply;
class QNetworkAccessManager;
class QListWidgetItem;

class Filter
{
  public:
    std::set<std::string> weapons;
    std::set<Gear::SkillType> cellSlots;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void closeEvent(QCloseEvent *event) override;

  private slots:
    void setProgress(int progress);
    void finishedSearch(ArmourSetSearch *ass);
  signals:
    void setProgressMainThread(int progress);
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
    void showArmourSets(const std::vector<Gear::ArmourSet> &armoursets);
    void showLoadedSearch();
    void saveSearchSettings();
    void saveSearch();
    void loadSearch();
    void updateNetworkReply(QNetworkReply *);
    void clearSearch();
    void applyFilter();

    Options options;
    std::vector<SkillSelector *> skillSelectors;
    Ui::MainWindow *ui;
    Dictionary dict;
    Gear::Armoury armoury;
    std::unordered_map<Gear::SkillType, std::vector<const Gear::SkillInfo *>> skills;
    QNetworkAccessManager *manager = NULL;
    bool cancel = false;
    QFutureWatcher<void> *searchWatcher = new QFutureWatcher<void>();
    Filter filter;
    bool isCreatingArmourSets = false;
    std::vector<QListWidgetItem *> armourSetItems;
};

#endif // !DAUNTLESS_ASS_UI_MAINWINDOW_HPP
