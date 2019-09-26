#include "ui/mainwindow.hpp"
#include "gear/ArmourSet.hpp"
#include "ui/About.hpp"
#include "ui/AdvancedSearch.hpp"
#include "ui/ArmourSetView.hpp"
#include "ui/CellWindow.hpp"
#include "ui/SkillSelector.hpp"
#include "ui/Translation.hpp"
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
    options.save(armoury);
    if (manager)
        delete manager;
    delete searchWatcher;
    delete ui;
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), dict(Dictionary()), armoury(dict)
{
    ui->setupUi(this);
    QSettings settings;
    restoreGeometry(settings.value(GEOMETRY).toByteArray());
    // create docks, toolbars, etc…
    restoreState(settings.value(STATE).toByteArray());

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
    // for (auto& filter : skills)
    //{
    //    std::sort(filter.second.begin(), filter.second.end(),
    //              [this](const Gear::SkillInfo* lhs, const Gear::SkillInfo* rhs) {
    //                  return dict.getTranslationFor(lhs->getName())
    //                             .compare(dict.getTranslationFor(rhs->getName())) < 0;
    //              });
    //}

    for (size_t i = 0; i < NUMBER_OF_SKILLSELECTORS; ++i)
    {
        auto selector = new SkillSelector(
            dict, skills, dynamic_cast<QGridLayout*>(ui->groupBoxWantedSkills->layout()), armoury);
        skillSelectors.push_back(selector);
    }

    connect(ui->pushButtonSearch, &QPushButton::clicked, [this]() { search(); });
    connect(ui->pushButtonAdvancedSearch, &QPushButton::clicked, [this]() { advancedSearch(); });
    connect(ui->pushButtonCancel, &QPushButton::clicked, [this]() { cancelSearch = true; });

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
        QAction* action = new QAction();
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

    ui->comboBoxCellUsage->setCurrentIndex(options.cellUsage);
    connect(ui->comboBoxCellUsage, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this](int value) { options.cellUsage = value; });

    auto updateLabel = new QLabel();
    ui->menuBar->setCornerWidget(updateLabel);
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished,
            [this](QNetworkReply* reply) { updateNetworkReply(reply); });
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
    connect(ui->comboBoxFilterAdditionalSkills, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this](int) { applyFilter(); });

    ui->actionUseLowerTierArmour->setChecked(options.useLowerTierArmour);
    connect(ui->actionUseLowerTierArmour, &QAction::triggered,
            [this](bool checked) { options.useLowerTierArmour = checked; });

    ui->comboBoxTier->setCurrentIndex(options.tier - 1);
    connect(ui->comboBoxTier, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this](int index) { options.tier = index + 1; });
    ui->comboBoxWeaponElement->setCurrentIndex(options.weaponElement);
    connect(ui->comboBoxWeaponElement, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this](int index) { options.weaponElement = Gear::Element(index); });
}

void MainWindow::setupTranslation()
{
    setWindowTitle(QString("ChaosSaber's Armour Set Search v") + PROGRAM_VERSION);
    ui->labelWeaponType->setText(getTranslation(dict, "label_weapon_type"));
    ui->comboBoxWeaponType->addItems(
        QStringList() << getTranslation(dict, "sword") << getTranslation(dict, "hammer")
                      << getTranslation(dict, "chainblades") << getTranslation(dict, "axe")
                      << getTranslation(dict, "pike") << getTranslation(dict, "repeater"));
    ui->labelWeaponElement->setText(getTranslation(dict, "label_weapon_element"));
    ui->labelWeaponElement->setToolTip(getTranslation(dict, "label_weapon_element_desc"));
    ui->comboBoxWeaponElement->addItems(
        QStringList() << getTranslation(dict, "element_all") << getTranslation(dict, "element_none")
                      << getTranslation(dict, "element_fire") << getTranslation(dict, "element_ice")
                      << getTranslation(dict, "element_shock")
                      << getTranslation(dict, "element_terra")
                      << getTranslation(dict, "element_umbral")
                      << getTranslation(dict, "element_radiant"));
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
    ui->comboBoxCellUsage->addItem(getTranslation(dict, "cell_usage_best"));
    ui->comboBoxCellUsage->addItem(getTranslation(dict, "cell_usage_own"));
    ui->actionClearSkills->setText(getTranslation(dict, "menu_clear_skills"));
    ui->pushButtonCancel->setText(getTranslation(dict, "button_cancel"));
    ui->labelFilterFreeCells->setText(getTranslation(dict, "label_filter_cells"));
    ui->labelFilterWeapon->setText(getTranslation(dict, "label_filter_weapon"));
    ui->labelFilterAdditionalSkills->setText(getTranslation(dict, "label_filter_skills"));
    ui->actionUseLowerTierArmour->setText(getTranslation(dict, "use_lower_tier_gear"));
    ui->labelTier->setText(getTranslation(dict, "label_tier"));
    ui->comboBoxTier->addItem(getTranslation(dict, "tier_t1"));
    ui->comboBoxTier->addItem(getTranslation(dict, "tier_t2"));
    ui->comboBoxTier->addItem(getTranslation(dict, "tier_t3"));

    // status bar (stats)
    auto addSpacer = [this]() {
        QWidget* spacer = new QWidget();
        spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        ui->statusBar->addWidget(spacer);
    };
    ui->statusBar->addWidget(new QLabel(getTranslation(dict, "stat_searched_combinations")));
    statCombinations = new QLabel("-/-");
    ui->statusBar->addWidget(statCombinations);
    addSpacer();
    ui->statusBar->addWidget(new QLabel(getTranslation(dict, "stat_found_sets")));
    statFoundSets = new QLabel("-");
    ui->statusBar->addWidget(statFoundSets);
    addSpacer();
    ui->statusBar->addWidget(new QLabel(getTranslation(dict, "stat_elapsed_time")));
    statElapsedTime = new QLabel("-");
    ui->statusBar->addWidget(statElapsedTime);
    addSpacer();
    ui->statusBar->addWidget(new QLabel(getTranslation(dict, "stat_searches_per_second")));
    statSearchesPerSecond = new QLabel("-");
    ui->statusBar->addWidget(statSearchesPerSecond);
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
        armoury, (Gear::WeaponType)ui->comboBoxWeaponType->currentIndex(), Gear::WantedSkillList(wantedSkills, armoury), options));
}

void MainWindow::advancedSearch()
{
    AdvancedSearch search((Gear::WeaponType)ui->comboBoxWeaponType->currentIndex(), armoury, dict,
                          options, Gear::WantedSkillList(getWantedSkills(), armoury), this);
    connect(&search, &AdvancedSearch::armourSetSearch, this,
            [this](ArmourSetSearch* ass) { armourSetSearch(ass); });
    search.setModal(true);
    search.exec();
}

void MainWindow::armourSetSearch(ArmourSetSearch* ass)
{
    Gear::AvailableCellList cells(armoury);
    // we try to minimize the cells in the list, so that we have less to iterate over in the search
    for (const auto& skillId : ass->getWantedSkills().getWantedSkills())
    {
        auto type = armoury.getSkillTypeFor(skillId);
        if (options.cellUsage == 0) // best cells
        {
            cells += Gear::Cell(Gear::Skill(skillId, 3), type) * 2;
        }
        else if (options.cellUsage == 1) // own cells
        {
            size_t sum = 0;
            for (size_t i = 3; i >= 1 && sum < 6; --i)
            {
                Gear::Cell cell(Gear::Skill(skillId, i), type);
                auto count = options.cells[cell];
                cells += cell * count;
                sum += count * i;
            }
        }
        else
        {
            std::cout << "unknown cell usage option" << options.cellUsage;
        }
    }
    ass->setProgressCallback(
        [this](ArmourSetSearch::SearchStatistics stats) { emit setProgressMainThread(stats); });
    ass->setAvaiableCells(std::move(cells));
    setSearchButtonsState(false);
    cancelSearch = false;
    connect(searchWatcher, &QFutureWatcher<void>::finished,
            [this, ass]() { emit finishedSearchMainThread(ass); });
    using namespace std::placeholders;
    QFuture<void> future = QtConcurrent::run(ass, &ArmourSetSearch::search, armoury, &cancelSearch);
    searchWatcher->setFuture(future);
}

void MainWindow::setNumberOfResults(QAction* caller)
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

void MainWindow::setLanguage(QAction* caller)
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
    ui->comboBoxFilterAdditionalSkills->clear();
    ui->comboBoxFilterAdditionalSkills->addItem("");
    filter.clear();
    armourSetItems.clear();
    std::stringstream ss;
    // TODO: add translation
    ss << "Found " << options.armourSets.size() << " Results";
    if (options.armourSets.size() > options.numberOfResults)
        ss << std::endl << "Displaying only the first " << options.numberOfResults << " results";
    ui->listWidgetArmourSets->addItem(QString::fromStdString(ss.str()));
    std::unordered_map<const Gear::ArmourSet*, ArmourSetView*> views;
    int maxWidthGear = 0;
    int maxWidthCells = 0;
    auto wantedSkills = getWantedSkills();
    for (const auto& set : options.armourSets)
    {
        // fill filter
        filter.weapons.insert(set.getWeapon().getName());
        for (const auto& cell : set.getCellList())
            if (cell.first.isEmpty())
                filter.cellSlots.insert(cell.first.getCellType());
        for (const auto& skill : set.getAdditionalSkills(wantedSkills))
        {
            filter.additionalSkills.insert(skill);
        }
        // create view
        if (views.size() > options.numberOfResults)
            continue;
        auto view = createArmourSetView(set, wantedSkills);
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
    for (const auto& weapon : filter.weapons)
        ui->comboBoxFilterWeapon->addItem(getTranslation(dict, weapon));
    for (auto cell : filter.cellSlots)
        if (cell != Gear::SkillType::None)
            ui->comboBoxFilterFreeCells->addItem(
                QString::fromStdString(Gear::SkillTypeToStringKey(cell)));
    for (const auto& skill : filter.additionalSkills)
        ui->comboBoxFilterAdditionalSkills->addItem(
            QString::fromStdString(skill.toString(dict, armoury)));
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
        options.saveSearch(armoury, fileName);
    }
    catch (const OptionsIoException& e)
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
    catch (const OptionsIoException& e)
    {
        QMessageBox box(QMessageBox::Critical, getTranslation(dict, "error"), e.what.c_str());
        box.exec();
    }
    showLoadedSearch();
    QFileInfo info(fileName);
    options.lastSearchSaveLocation = info.path();
}

void MainWindow::updateNetworkReply(QNetworkReply* reply)
{
    auto label = dynamic_cast<QLabel*>(ui->menuBar->cornerWidget());
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

void MainWindow::closeEvent(QCloseEvent* event)
{
    cancelSearch = true;
    QSettings settings;
    settings.setValue(GEOMETRY, saveGeometry());
    settings.setValue(STATE, saveState());
    QWidget::closeEvent(event);
}

void MainWindow::setProgress(ArmourSetSearch::SearchStatistics stats)
{
    ui->progressBarSearch->setValue(stats.progress);
    const std::chrono::duration<double> diff = stats.end.load() - stats.start.load();
    statCombinations->setText(QString::number(stats.searchedCombinations) + "/" +
                              QString::number(stats.possibleCombinations));
    statFoundSets->setText(QString::number(stats.foundSets));
    statElapsedTime->setText(QString::number(diff.count()));
    statSearchesPerSecond->setText(QString::number(stats.searchedCombinations / diff.count()));
}

void MainWindow::finishedSearch(ArmourSetSearch* ass)
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
    Gear::Skill additionalSkill;

    if (ui->comboBoxFilterWeapon->currentIndex() != 0)
        weapon = *std::next(filter.weapons.begin(), ui->comboBoxFilterWeapon->currentIndex() - 1);
    if (ui->comboBoxFilterFreeCells->currentIndex() != 0)
        type =
            *std::next(filter.cellSlots.begin(), ui->comboBoxFilterFreeCells->currentIndex() - 1);
    if (ui->comboBoxFilterAdditionalSkills->currentIndex() != 0)
        additionalSkill = *std::next(filter.additionalSkills.begin(),
                                     ui->comboBoxFilterAdditionalSkills->currentIndex() - 1);

    auto wantedSkills = getWantedSkills();
    size_t count = 0;
    for (int i = 0; i < armourSetItems.size() && i < options.numberOfResults; ++i)
    {
        bool hide = false;
        if (!weapon.empty() && weapon != options.armourSets[i].getWeapon().getName())
            hide = true;
        if (type != Gear::SkillType::None)
        {
            if (!options.armourSets[i].hasFreeCellSlotFor(type))
                hide = true;
        }
        if (additionalSkill.getSkillPoints() != 0)
        {
            if (!options.armourSets[i].getAdditionalSkills(wantedSkills).contains(additionalSkill))
                hide = true;
        }
        const auto& set = options.armourSets[i];
        if (armourSetItems.count(&set) > 0)
        {
            armourSetItems.at(&set)->setHidden(hide);
        }
        else
        {
            createArmourSetItem(&set, createArmourSetView(set, wantedSkills));
        }
        if (!hide)
            ++count;
    }
}

void Filter::clear()
{
    weapons.clear();
    cellSlots.clear();
    additionalSkills.clear();
}

void MainWindow::createArmourSetItem(const Gear::ArmourSet* set, ArmourSetView* view)
{
    view->setGearViewWidth(armourSetViewGearWidth);
    view->setCellViewWidth(armourSetViewCellWidth);
    auto item = new QListWidgetItem();
    armourSetItems.insert({set, item});
    item->setSizeHint(view->sizeHint());
    ui->listWidgetArmourSets->addItem(item);
    ui->listWidgetArmourSets->setItemWidget(item, view);
}

ArmourSetView* MainWindow::createArmourSetView(const Gear::ArmourSet& set,
                                               std::vector<Gear::Skill> wantedSkills)
{
    return new ArmourSetView(dict, options, set, armoury,
                             ui->listWidgetArmourSets->verticalScrollBar()->sizeHint().width(),
                             Gear::SkillList(wantedSkills));
}

void MainWindow::saveDataFiles(QNetworkReply* reply, const QString& fileName) {}