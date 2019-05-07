#include "gear/ArmourSet.hpp"
#include "ui/About.hpp"
#include "ui/AdvancedSearch.hpp"
#include "ui/ArmourSetView.hpp"
#include "ui/CellWindow.hpp"
#include "ui/SkillSelector.hpp"
#include "ui/Translation.hpp"
#include "ui/mainwindow.hpp"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QFutureWatcher>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QScrollBar>
#include <QSettings>
#include <QUrl>
#include <QtConcurrent>
#include <iostream>
#include <sstream>

#define ORG "ChaosFoundary"
#define APP_NAME "ChaosSaber's ASS"
#define GEOMETRY "MainWindow/Geometry"
#define STATE "MainWindow/State"

MainWindow::~MainWindow()
{
    saveSearchSettings();
    options.save();
    if (manager)
        delete manager;
    delete searchWatcher;
    delete ui;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), dict(Dictionary()), armoury(dict)
{
    ui->setupUi(this);
    QSettings settings;
    restoreGeometry(settings.value(GEOMETRY).toByteArray());
    // create docks, toolbars, etc…
    restoreState(settings.value(STATE).toByteArray());

    // TODO: Download Data files if not exist?
    ui->actionCheckForUpdates->setEnabled(false);
    ui->actionDownloadNewestData->setEnabled(false);

    armoury.loadData();
    // TODO: maybe a nothrow option?
    try
    {
        options.loadConfiguration(armoury);
    }
    catch (OptionsIoException)
    {
    }
    dict.loadLanguage(options.language);
    try
    {
        options.loadCells(armoury, dict);
        options.loadSearch(armoury, dict);
    }
    catch (OptionsIoException)
    {
    }

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
    connect(ui->pushButtonCancel, &QPushButton::clicked, [this]() { cancel = true; });

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

    auto updateLabel = new QLabel();
    ui->menuBar->setCornerWidget(updateLabel);
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished,
            [this](QNetworkReply *reply) { updateNetworkReply(reply); });
    manager->get(QNetworkRequest(
        QUrl("https://github.com/ChaosSaber/ChaosSabers-Armour-Set-Search/releases/latest")));

    connect(ui->actionClearSkills, &QAction::triggered, [this](bool) { clearSearch(); });
    ui->widgetSearch->setMaximumWidth(ui->widgetSearch->sizeHint().width() * 1.1);

    connect(this, &MainWindow::setProgressMainThread, this, &MainWindow::setProgress);
    connect(this, &MainWindow::finishedSearchMainThread, this, &MainWindow::finishedSearch);
    connect(ui->comboBoxFilterFreeCells, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this](int) { applyFilter(); });
    connect(ui->comboBoxFilterWeapon, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this](int) { applyFilter(); });

    ui->actionUseLowerTierArmour->setChecked(options.useLowerTierArmour);
    connect(ui->actionUseLowerTierArmour, &QAction::triggered,
            [this](bool checked) { options.useLowerTierArmour = checked; });

    ui->comboBoxTier->addItem(getTranslation(dict, "tier_t1"));
    ui->comboBoxTier->addItem(getTranslation(dict, "tier_t2"));
    ui->comboBoxTier->addItem(getTranslation(dict, "tier_t3"));
    ui->comboBoxTier->setCurrentIndex(options.tier - 1);
    connect(ui->comboBoxTier, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this](int index) { options.tier = index + 1; });
}

void MainWindow::setupTranslation()
{
    setWindowTitle(QString("ChaosSaber's Armour Set Search v") + PROGRAM_VERSION);
    ui->labelWeaponType->setText(getTranslation(dict, "label_weapon_type"));
    ui->comboBoxWeaponType->addItems(
        QStringList() << getTranslation(dict, "sword") << getTranslation(dict, "hammer")
                      << getTranslation(dict, "chainblades") << getTranslation(dict, "axe")
                      << getTranslation(dict, "pike") << getTranslation(dict, "repeater"));
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
    ui->groupBoxCells->setTitle(getTranslation(dict, "label_cells"));
    ui->pushButtonOrganizeCells->setText(getTranslation(dict, "button_organize"));
    ui->actionClearSkills->setText(getTranslation(dict, "menu_clear_skills"));
    ui->pushButtonCancel->setText(getTranslation(dict, "button_cancel"));
    ui->labelFilterFreeCells->setText(getTranslation(dict, "label_filter_cells"));
    ui->labelFilterWeapon->setText(getTranslation(dict, "label_filter_weapon"));
    ui->actionUseLowerTierArmour->setText(getTranslation(dict, "use_lower_tier_gear"));
    ui->labelTier->setText(getTranslation(dict, "label_tier"));
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
    ui->pushButtonCancel->setEnabled(!enable);
}

void MainWindow::search()
{
    std::vector<Gear::Skill> wantedSkills = getWantedSkills();
    if (wantedSkills.empty())
        return;
    armourSetSearch(new ArmourSetSearch(
        armoury, (Gear::WeaponType)ui->comboBoxWeaponType->currentIndex(), wantedSkills, options));
}

void MainWindow::advancedSearch()
{
    AdvancedSearch search((Gear::WeaponType)ui->comboBoxWeaponType->currentIndex(), armoury, dict,
                          options, getWantedSkills(), this);
    connect(&search, &AdvancedSearch::armourSetSearch, this,
            [this](ArmourSetSearch *ass) { armourSetSearch(ass); });
    search.setModal(true);
    search.exec();
}

void MainWindow::armourSetSearch(ArmourSetSearch *ass)
{
    Gear::CellList cells;
    for (const auto &skill : ass->getWantedSkills())
    {
        auto type = armoury.getSkillTypeFor(skill.getName());
        for (int i = 1; i <= 3; ++i)
        {
            Gear::Cell cell(Gear::Skill(skill.getName(), i), type);
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
    ass->setAvaiableCells(cells);
    setSearchButtonsState(false);
    cancel = false;
    connect(searchWatcher, &QFutureWatcher<void>::finished,
            [this, ass]() { emit finishedSearchMainThread(ass); });
    using namespace std::placeholders;
    QFuture<void> future =
        QtConcurrent::run(ass, &ArmourSetSearch::search, armoury, &cancel,
                          [this](int progress) { emit setProgressMainThread(progress); });
    searchWatcher->setFuture(future);
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

void MainWindow::showArmourSets()
{
    isCreatingArmourSets = true;
    ui->listWidgetArmourSets->clear();
    ui->comboBoxFilterFreeCells->clear();
    ui->comboBoxFilterFreeCells->addItem("");
    ui->comboBoxFilterWeapon->clear();
    ui->comboBoxFilterWeapon->addItem("");
    filter.clear();
    armourSetItems.clear();
    std::stringstream ss;
    // TODO: add translation
    ss << "Found " << options.armourSets.size() << " Results";
    if (options.armourSets.size() > options.numberOfResults)
        ss << std::endl << "Displaying only the first " << options.numberOfResults << " results";
    ui->listWidgetArmourSets->addItem(QString::fromStdString(ss.str()));
    std::unordered_map<const Gear::ArmourSet *, ArmourSetView *> views;
    int maxWidthGear = 0;
    int maxWidthCells = 0;
    for (const auto &set : options.armourSets)
    {
        filter.weapons.insert(set.getWeapon().getName());
        for (const auto &cell : set.getCellList())
            if (cell.first.isEmpty())
                filter.cellSlots.insert(cell.first.getCellType());
        if (views.size() > options.numberOfResults)
            continue;
        auto view = createArmourSetView(set);
        views.insert({&set, view});
        if (view->getGearViewWidth() > maxWidthGear)
            maxWidthGear = view->getGearViewWidth();
        if (view->getCellViewWidth() > maxWidthCells)
            maxWidthCells = view->getCellViewWidth();
    }
    armourSetViewGearWidth = maxWidthGear;
    armourSetViewCellWidth = maxWidthCells;
    for (auto setView : views)
        createArmourSetItem(setView.first, setView.second);
    ui->listWidgetArmourSets->setMinimumWidth(ui->listWidgetArmourSets->sizeHintForColumn(0));
    for (const auto &weapon : filter.weapons)
        ui->comboBoxFilterWeapon->addItem(getTranslation(dict, weapon));
    for (auto cell : filter.cellSlots)
        ui->comboBoxFilterFreeCells->addItem(
            QString::fromStdString(Gear::SkillTypeToStringKey(cell)));
    isCreatingArmourSets = false;
}

void MainWindow::showLoadedSearch()
{
    ui->comboBoxWeaponType->setCurrentIndex(options.weaponType);
    for (size_t i = 0; i < NUMBER_OF_SKILLSELECTORS; ++i)
        skillSelectors[i]->set(options.skillSearches[i]);
    if (options.armourSets.size() > 0)
        showArmourSets();
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
                                                 options.lastSearchSaveLocation, filter, &filter);
    if (fileName.isEmpty())
        return;
    saveSearchSettings();
    QFileInfo info(fileName);
    options.lastSearchSaveLocation = info.path();
    try
    {
        options.saveSearch(fileName);
    }
    catch (const OptionsIoException &e)
    {
        QMessageBox box(QMessageBox::Critical, getTranslation(dict, "error"), e.what.c_str());
        box.exec();
    }
}

void MainWindow::loadSearch()
{
    auto fileName = QFileDialog::getOpenFileName(this, getTranslation(dict, "menu_load"),
                                                 options.lastSearchSaveLocation,
                                                 "Armour Set Searches (*.ass);;All Files(*.*)");
    if (fileName.isEmpty())
        return;
    try
    {
        options.loadSearch(armoury, dict, fileName);
    }
    catch (const OptionsIoException &e)
    {
        QMessageBox box(QMessageBox::Critical, getTranslation(dict, "error"), e.what.c_str());
        box.exec();
    }
    showLoadedSearch();
    QFileInfo info(fileName);
    options.lastSearchSaveLocation = info.path();
}

void MainWindow::updateNetworkReply(QNetworkReply *reply)
{
    auto label = dynamic_cast<QLabel *>(ui->menuBar->cornerWidget());
    auto attr = reply->attribute(QNetworkRequest::Attribute::RedirectionTargetAttribute);
    if (reply->error() != QNetworkReply::NoError || !attr.isValid() ||
        (attr.userType() != QMetaType::QUrl))
    {
        return;
    }
    auto url = attr.toUrl();
    if (url.isRelative())
    {
        url = reply->url().resolved(url);
    }
    if (!url.toString().endsWith(PROGRAM_VERSION))
    {
        QString text = "<a href=\"";
        text += url.toString();
        text += "\">";
        text += getTranslation(dict, "update_new_version");
        text += " </a>";
        label->setText(text);
        label->setTextFormat(Qt::RichText);
        label->setTextInteractionFlags(Qt::TextBrowserInteraction);
        label->setOpenExternalLinks(true);
    }

    ui->menuBar->setCornerWidget(label);
    reply->deleteLater();
}

void MainWindow::clearSearch()
{
    for (auto selector : skillSelectors)
        selector->clear();
    options.checkedGear.clear();
    options.armourSets.clear();
    ui->listWidgetArmourSets->clear();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.setValue(GEOMETRY, saveGeometry());
    settings.setValue(STATE, saveState());
    QWidget::closeEvent(event);
}

void MainWindow::setProgress(int progress) { ui->progressBarSearch->setValue(progress); }

void MainWindow::finishedSearch(ArmourSetSearch *ass)
{
    options.armourSets = ass->moveArmourSets();
    delete ass;
    showArmourSets();
    setSearchButtonsState(true);
    searchWatcher->disconnect();
}

void MainWindow::applyFilter()
{
    if (isCreatingArmourSets)
        return;
    std::string weapon = "";
    Gear::SkillType type = Gear::SkillType::None;

    if (ui->comboBoxFilterWeapon->currentIndex() != 0)
        weapon = *std::next(filter.weapons.begin(), ui->comboBoxFilterWeapon->currentIndex() - 1);
    if (ui->comboBoxFilterFreeCells->currentIndex() != 0)
        type =
            *std::next(filter.cellSlots.begin(), ui->comboBoxFilterFreeCells->currentIndex() - 1);

    size_t count = 0;
    for (int i = 0; i < armourSetItems.size() && i < options.numberOfResults; ++i)
    {
        bool hide = false;
        if (weapon != "" && weapon != options.armourSets[i].getWeapon().getName())
            hide = true;
        if (type != Gear::SkillType::None)
        {
            bool foundCell = false;
            for (const auto &cell : options.armourSets[i].getCellList())
                if (cell.first.isEmpty() && cell.first.getCellType() == type)
                    foundCell = true;
            if (!foundCell)
                hide = true;
        }
        const auto &set = options.armourSets[i];
        if (armourSetItems.count(&set) > 0)
        {
            armourSetItems.at(&set)->setHidden(hide);
        }
        else
        {
            createArmourSetItem(&set, createArmourSetView(set));
        }
        if (!hide)
            ++count;
    }
}

void Filter::clear()
{
    weapons.clear();
    cellSlots.clear();
}

void MainWindow::createArmourSetItem(const Gear::ArmourSet *set, ArmourSetView *view)
{
    view->setGearViewWidth(armourSetViewGearWidth);
    view->setCellViewWidth(armourSetViewCellWidth);
    auto item = new QListWidgetItem();
    armourSetItems.insert({set, item});
    item->setSizeHint(view->sizeHint());
    ui->listWidgetArmourSets->addItem(item);
    ui->listWidgetArmourSets->setItemWidget(item, view);
}

ArmourSetView *MainWindow::createArmourSetView(const Gear::ArmourSet &set)
{
    return new ArmourSetView(dict, options, set, armoury,
                             ui->listWidgetArmourSets->verticalScrollBar()->sizeHint().width(),
                             Gear::SkillList(getWantedSkills()));
}

void MainWindow::saveDataFiles(QNetworkReply *reply, const QString &fileName) {}