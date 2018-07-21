#include "gear/ArmourSet.hpp"
#include "ui/About.hpp"
#include "ui/AdvancedSearch.hpp"
#include "ui/ArmourSetView.hpp"
#include "ui/CellWindow.hpp"
#include "ui/SkillSelector.hpp"
#include "ui/Translation.hpp"
#include "ui/mainwindow.hpp"
#include "ui_mainwindow.h"
#include <iostream>
#include <QDesktopServices>
#include <QFileDialog>
#include <QFileInfo>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QUrl>
#include <sstream>

MainWindow::~MainWindow()
{
    saveSearchSettings();
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

    for (size_t i = 0; i < NUMBER_OF_SKILLSELECTORS; ++i)
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

    connect(ui->actionUpdates, &QAction::triggered, [](bool) {
        QDesktopServices::openUrl(
            QUrl("https://github.com/ChaosSaber/ChaosSabers-Armour-Set-Search/releases/latest"));
    });
    showLoadedSearch();
    connect(ui->actionLoadSearch, &QAction::triggered, [this](bool) { loadSearch(); });
    connect(ui->actionSaveSearch, &QAction::triggered, [this](bool) { saveSearch(); });
    connect(ui->pushButtonOrganizeCells, &QPushButton::clicked, [this]() {
        CellWindow dialog(armoury, options, dict, this);
        dialog.exec();
    });

    ui->comboBoxCellUsage->addItem(getTranslation(dict, "cell_usage_best"));
    ui->comboBoxCellUsage->addItem(getTranslation(dict, "cell_usage_own"));
    ui->comboBoxCellUsage->setCurrentIndex(options.cellUsage);
    connect(ui->comboBoxCellUsage, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this](int value) { options.cellUsage = value; });
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
    ui->actionUpdates->setText(getTranslation(dict, "menu_updates"));
    ui->groupBoxCells->setTitle(getTranslation(dict, "label_cells"));
    ui->pushButtonOrganizeCells->setText(getTranslation(dict, "button_organize"));
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
    armourSetSearch(ArmourSetSearch(
        armoury, (Gear::WeaponType)ui->comboBoxWeaponType->currentIndex(), wantedSkills));
}

void MainWindow::advancedSearch()
{
    AdvancedSearch search((Gear::WeaponType)ui->comboBoxWeaponType->currentIndex(), armoury, dict,
                          options, getWantedSkills(), this);
    connect(&search, &AdvancedSearch::armourSetSearch, this,
                [this](ArmourSetSearch ass) { armourSetSearch(ass); });
    search.setModal(true);
    search.exec();
}

void MainWindow::armourSetSearch(ArmourSetSearch &ass)
{   
    Gear::CellList cells;
    for (const auto &skill : ass.getWantedSkills())
    {
        for (size_t i = 1; i <= 3; ++i)
        {
            Gear::Cell cell(Gear::Skill(skill.getName(), i),
                            armoury.getSkillTypeFor(skill.getName()));
            if (options.cellUsage == 0) // bestCells
            {
                cells += cell * (6 / i);
            }
            else if (options.cellUsage == 1) // own cells
            {
                cells += cell * options.cells[cell];
            }
            else
            {
                std::cout << "unknown cell usage option" << options.cellUsage;
            }
        }
    }
    ass.setAvaiableCells(cells);
    setSearchButtonsState(false);
    ass.search(armoury, [this](unsigned long long count, unsigned long long max) {
        ui->progressBarSearch->setValue(100 * count / max);
    });
    showArmourSets(ass.getArmourSets());
    options.armourSets = ass.getArmourSets();
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

void MainWindow::showArmourSets(const std::vector<Gear::ArmourSet> &armoursets)
{
    ui->listWidgetArmourSets->clear();
    std::stringstream ss;
    // TODO: add translation
    ss << "Found " << armoursets.size() << " Results";
    if (armoursets.size() > options.numberOfResults)
        ss << std::endl << "Displaying only the first " << options.numberOfResults << " results";
    ui->listWidgetArmourSets->addItem(QString::fromStdString(ss.str()));
    size_t count = 0;
    for (const auto &set : armoursets)
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
}

void MainWindow::showLoadedSearch()
{
    ui->comboBoxWeaponType->setCurrentIndex(options.weaponType);
    for (size_t i = 0; i < NUMBER_OF_SKILLSELECTORS; ++i)
        skillSelectors[i]->set(options.skillSearches[i]);
    showArmourSets(options.armourSets);
}

void MainWindow::saveSearchSettings()
{
    options.weaponType = (Gear::WeaponType)ui->comboBoxWeaponType->currentIndex();
    for (size_t i = 0; i < NUMBER_OF_SKILLSELECTORS; ++i)
        options.skillSearches[i] = std::move(skillSelectors[i]->getSearchSettings());
}

void MainWindow::saveSearch()
{
    QString filter = "Armour Set Searches (*.ass)";
    auto fileName = QFileDialog::getSaveFileName(this, getTranslation(dict, "menu_save"),
                                                 options.lastSaveLocation, filter, &filter);
    if (fileName.isEmpty())
        return;
    saveSearchSettings();
    QFileInfo info(fileName);
    options.lastSaveLocation = info.path();
    try
    {
        options.saveSearch(fileName.toStdString());
    }
    catch (const OptionsIoException &e)
    {
        QMessageBox box(QMessageBox::Critical, getTranslation(dict, "error"), e.what.c_str());
    }
}

void MainWindow::loadSearch()
{
    auto fileName = QFileDialog::getOpenFileName(this, getTranslation(dict, "menu_load"),
                                                 options.lastSaveLocation,
                                                 "Armour Set Searches (*.ass);;All Files(*.*)");
    if (fileName.isEmpty())
        return;
    try
    {
        options.loadSearch(armoury, fileName.toStdString());
    }
    catch (const OptionsIoException &e)
    {
        QMessageBox box(QMessageBox::Critical, getTranslation(dict, "error"), e.what.c_str());
    }
    showLoadedSearch();
    QFileInfo info(fileName);
    options.lastSaveLocation = info.path();
}
