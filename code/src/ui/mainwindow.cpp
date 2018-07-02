#include "ui/mainwindow.hpp"
#include "gear/ArmourSet.hpp"
#include "ui/About.hpp"
#include "ui/AdvancedSearch.hpp"
#include "ui/ArmourSetView.hpp"
#include "ui/SkillSelector.hpp"
#include "ui/Translation.hpp"
#include "ui_mainwindow.h"
#include <QDesktopServices>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QUrl>
#include <sstream>

MainWindow::~MainWindow()
{
    options.save();
    delete ui;
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    options.load(armoury);
    dict.loadLanguage(options.language);
    setupTranslation();

    skills[Gear::SkillType::None] = armoury.getSkills(Gear::SkillType::None);
    skills[Gear::SkillType::Defense] = armoury.getSkills(Gear::SkillType::Defense);
    skills[Gear::SkillType::Power] = armoury.getSkills(Gear::SkillType::Power);
    skills[Gear::SkillType::Technique] = armoury.getSkills(Gear::SkillType::Technique);
    skills[Gear::SkillType::Mobility] = armoury.getSkills(Gear::SkillType::Mobility);
    skills[Gear::SkillType::Utility] = armoury.getSkills(Gear::SkillType::Utility);
    for (auto &filter : skills)
    {
        std::sort(filter.second.begin(), filter.second.end(),
                  [this](const Gear::SkillInfo *lhs, const Gear::SkillInfo *rhs) {
                      return dict.getTranslationFor(lhs->getName())
                                 .compare(dict.getTranslationFor(rhs->getName())) < 0;
                  });
    }

    for (size_t i = 0; i < 7; ++i)
    {
        auto selector = new SkillSelector(
            dict, skills, dynamic_cast<QGridLayout *>(ui->groupBoxWantedSkills->layout()));
        skillSelectors.push_back(selector);
    }

    connect(ui->pushButtonSearch, &QPushButton::clicked, [this]() { search(); });
    connect(ui->pushButtonAdvancedSearch, &QPushButton::clicked, [this]() { advancedSearch(); });

    ui->listWidgetArmourSets->setStyleSheet(
        "QListWidget::item { border-bottom: 1px solid black; }");
    for (auto action : ui->menuNumberOfResults->actions())
    {
        if (action->text() == QString::fromStdString(std::to_string(options.numberOfResults)))
            action->setChecked(true);
        connect(action, &QAction::triggered, [this, action](bool) { setNumberOfResults(action); });
    }
    for (const auto language : dict.getAvaiableLanguages())
    {
        QAction *action = new QAction();
        action->setCheckable(true);
        action->setText(QString::fromStdString(language));
        if (language == options.language)
            action->setChecked(true);
        ui->menuLanguage->addAction(action);
        connect(action, &QAction::triggered, [this, action](bool) { setLanguage(action); });
    }

    connect(ui->actionAboutQt, &QAction::triggered,
            [this](bool) { QMessageBox::aboutQt(this, getTranslation(dict, "menu_about_qt")); });
    connect(ui->actionAbout, &QAction::triggered, [this](bool) { about(); });

    connect(ui->actionUpdates, &QAction::triggered,
            [](bool) { QDesktopServices::openUrl(QUrl("https://github.com/ChaosSaber/Dauntless-Armour-Set-Search")); });
}

void MainWindow::setupTranslation()
{
    ui->labelWeaponType->setText(getTranslation(dict, "label_weapon_type"));
    ui->comboBoxWeaponType->addItems(
        QStringList() << getTranslation(dict, "sword") << getTranslation(dict, "hammer")
                      << getTranslation(dict, "chainblades") << getTranslation(dict, "axe")
                      << getTranslation(dict, "pike"));
    ui->groupBoxWantedSkills->setTitle(getTranslation(dict, "label_wanted_skills"));
    ui->pushButtonSearch->setText(getTranslation(dict, "button_search"));
    ui->pushButtonAdvancedSearch->setText(getTranslation(dict, "button_advanced_search"));
    ui->labelSkillTypeFilter->setText(getTranslation(dict, "label_skill_filter"));
    ui->labelSkillName->setText(getTranslation(dict, "label_skill_name"));
    ui->labelSkillLevel->setText(getTranslation(dict, "label_skill_level"));
    ui->menuFile->setTitle(getTranslation(dict, "menu_file"));
    ui->menuOptions->setTitle(getTranslation(dict, "menu_options"));
    ui->menuNumberOfResults->setTitle(getTranslation(dict, "menu_number_of_results"));
    ui->actionSaveSearch->setText(getTranslation(dict, "menu_save"));
    ui->actionLoadSearch->setText(getTranslation(dict, "menu_load"));
    ui->menuHelp->setTitle(getTranslation(dict, "menu_help"));
    ui->actionAbout->setText(getTranslation(dict, "menu_about"));
    ui->actionAboutQt->setText(getTranslation(dict, "menu_about_qt"));
}

std::vector<Gear::Skill> MainWindow::getWantedSkills()
{
    std::vector<Gear::Skill> wantedSkills;
    for (auto selector : skillSelectors)
        selector->getSkill(wantedSkills);
    return wantedSkills;
}

void MainWindow::setSearchButtonsState(bool enable)
{
    ui->pushButtonSearch->setEnabled(enable);
    ui->pushButtonAdvancedSearch->setEnabled(enable);
}

void MainWindow::search()
{
    std::vector<Gear::Skill> wantedSkills = getWantedSkills();
    if (wantedSkills.empty())
        return;
    Gear::CellList cells;
    for (auto skill : wantedSkills)
    {
        cells += Gear::Cell(Gear::Skill(skill.getName(), 3),
                            armoury.getSkillInfoFor(skill.getName()).getType()) *
                 2;
        cells += Gear::Cell(Gear::Skill(skill.getName(), 2),
                            armoury.getSkillInfoFor(skill.getName()).getType()) *
                 3;
        cells += Gear::Cell(Gear::Skill(skill.getName(), 1),
                            armoury.getSkillInfoFor(skill.getName()).getType()) *
                 6;
    }
    armourSetSearch(ArmourSetSearch(
        armoury, (Gear::WeaponType)ui->comboBoxWeaponType->currentIndex(), wantedSkills, cells));
}

void MainWindow::advancedSearch()
{
    auto advancedSearch =
        new AdvancedSearch((Gear::WeaponType)ui->comboBoxWeaponType->currentIndex(), armoury, dict,
                           options, getWantedSkills(), this);
    connect(advancedSearch, &AdvancedSearch::armourSetSearch,
            [this](ArmourSetSearch ass) { armourSetSearch(ass); });
    advancedSearch->exec();
}

void MainWindow::armourSetSearch(ArmourSetSearch &ass)
{
    setSearchButtonsState(false);
    ui->listWidgetArmourSets->clear();
    ass.search(armoury, [this](unsigned long long count, unsigned long long max) {
        ui->progressBarSearch->setValue(100 * count / max);
    });
    const auto &sets = ass.getArmourSets();
    std::stringstream ss;
    // TODO: add translation
    ss << "Found " << sets.size() << " Results";
    if (sets.size() > options.numberOfResults)
        ss << std::endl << "Displaying only the first " << options.numberOfResults << " results";
    ui->listWidgetArmourSets->addItem(QString::fromStdString(ss.str()));
    size_t count = 0;
    for (const auto &set : sets)
    {
        if (count > options.numberOfResults)
            break;
        auto item = new QListWidgetItem();
        auto view = new ArmourSetView(dict, set);
        item->setSizeHint(view->sizeHint());
        ui->listWidgetArmourSets->addItem(item);
        ui->listWidgetArmourSets->setItemWidget(item, view);
        ++count;
    }
    setSearchButtonsState(true);
}

void MainWindow::setNumberOfResults(QAction *caller)
{
    for (auto action : ui->menuNumberOfResults->actions())
    {
        if (action != caller)
            action->setChecked(false);
        else
            action->setChecked(true);
    }
    options.numberOfResults = caller->text().toInt();
}

void MainWindow::setLanguage(QAction *caller)
{
    for (auto action : ui->menuLanguage->actions())
    {
        if (action != caller)
            action->setChecked(false);
        else
            action->setChecked(true);
    }
    options.language = caller->text().toStdString();
}

void MainWindow::about()
{
    auto about = new About(dict, this);
    about->exec();
}