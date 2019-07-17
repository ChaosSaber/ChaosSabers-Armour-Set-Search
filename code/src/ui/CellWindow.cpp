#include "ui/CellWindow.hpp"
#include "ui/Translation.hpp"
#include "ui_CellWindow.h"

#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QResizeEvent>
#include <QSettings>
#include <sstream>

#define GEOMETRY "CellWindow/Geometry"

CellWindow::CellWindow(const Gear::Armoury &armoury, Options &options, const Dictionary &dict,
                       QWidget *parent)
    : QDialog(parent), options(options), ui(new Ui::CellWindow), dict(dict), armoury(armoury)
{
    ui->setupUi(this);
    QSettings settings;
    restoreGeometry(settings.value(GEOMETRY).toByteArray());

    auto widget = new QWidget();
    gridLayout = new QGridLayout();
    widget->setLayout(gridLayout);
    ui->scrollAreaCells->setWidget(widget);
    for (const auto &skill : armoury.getSkills(Gear::SkillType::None))
    {
        auto layout = new QGridLayout();
        auto widget = new QWidget();
        widget->setLayout(layout);
        auto label = new QLabel();
        label->setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);
        label->setText(getTranslation(dict, skill->getName()));
        Qt::Alignment alignment;
        alignment.setFlag(Qt::AlignmentFlag::AlignCenter, true);
        layout->addWidget(label, 0, 0, 1, 2, alignment);
        int row = 1;
        for (int i = 3; i > 0; --i)
        {
            Gear::Cell cell(Gear::Skill(armoury.getSkillIdForName(skill->getName()), i), skill->getType());

            auto spinBox = new QSpinBox();
            spinBox->setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);
            cellSpinBoxen[cell] = spinBox;
            layout->addWidget(spinBox, row, 1);
            spinBox->setMinimum(0);
            spinBox->setValue(options.cells[cell]);
            connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                    [this](int) { unsavedChanges = true; });

            auto label = new QLabel();
            label->setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);
            std::stringstream ss;
            ss << "+" << i;
            label->setText(QString::fromStdString(ss.str()));
            layout->addWidget(label, row, 0);

            ++row;
        }
        widgets[label->text()] = widget;
        if (widget->sizeHint().width() > maximumCellWidth)
            maximumCellWidth = widget->sizeHint().width();
        widget->setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum);
    }
    for (const auto &widget : widgets)
        widget.second->setMinimumWidth(maximumCellWidth);

    connect(ui->pushButtonCancel, &QPushButton::clicked, [this]() { close(); });
    connect(ui->pushButtonSave, &QPushButton::clicked, [this]() { save(); });
    connect(ui->pushButtonImport, &QPushButton::clicked, [this]() { importCells(); });
    connect(ui->pushButtonExport, &QPushButton::clicked, [this]() { exportCells(); });

    ui->pushButtonCancel->setText(getTranslation(dict, "button_cancel"));
    ui->pushButtonSave->setText(getTranslation(dict, "button_save"));
    ui->pushButtonImport->setText(getTranslation(dict, "button_import"));
    ui->pushButtonExport->setText(getTranslation(dict, "button_export"));

    connect(ui->lineEditCellFilter, &QLineEdit::textChanged,
            [this](QString) { layoutCells(gridLayout->columnCount()); });
}

CellWindow::~CellWindow() { delete ui; }

void CellWindow::layoutCells(int coloumns)
{
    if (coloumns == 0)
        coloumns = 1;
    int count = 0;
    for (auto widget : widgets)
    {
        gridLayout->removeWidget(widget.second);
        if (widget.first.startsWith(ui->lineEditCellFilter->text(), Qt::CaseInsensitive))
        {
            gridLayout->addWidget(widget.second, count / coloumns, count % coloumns,
                                  Qt::AlignTop | Qt::AlignLeft);
            widget.second->show();
            ++count;
        }
        else
            widget.second->hide();
    }
}

void CellWindow::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    if ((ui->scrollAreaCells->width() / maximumCellWidth) != gridLayout->columnCount())
        layoutCells(ui->scrollAreaCells->width() / maximumCellWidth);
}

void CellWindow::closeEvent(QCloseEvent *event)
{
    if (unsavedChanges)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, getTranslation(dict, "quit"),
                                      getTranslation(dict, "unsaved_changes_quit"),
                                      QMessageBox::Yes | QMessageBox::No);
        if (reply != QMessageBox::Yes)
        {
            event->ignore();
            return;
        }
    }

    QSettings settings;
    settings.setValue(GEOMETRY, saveGeometry());
    QDialog::closeEvent(event);
}

void CellWindow::save()
{
    for (const auto cellbox : cellSpinBoxen)
        options.cells[cellbox.first] = cellbox.second->value();
    unsavedChanges = false;
}

void CellWindow::importCells()
{
    auto fileName =
        QFileDialog::getOpenFileName(this, getTranslation(dict, "button_import"),
                                     options.lastCellSaveLocation, "Cells (*.cells);;All Files(*.*)");
    if (fileName.isEmpty())
        return;
    try
    {
        options.loadCells(armoury, dict, fileName);
    }
    catch (const OptionsIoException &e)
    {
        QMessageBox box(QMessageBox::Critical, getTranslation(dict, "error"), e.what.c_str());
        box.exec();
    }
    for (const auto &cellBox : cellSpinBoxen)
        cellBox.second->setValue(options.cells[cellBox.first]);
    unsavedChanges = false;
    QFileInfo info(fileName);
    options.lastCellSaveLocation = info.path();
}

void CellWindow::exportCells()
{
    if (unsavedChanges)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, getTranslation(dict, "quit"),
                                      getTranslation(dict, "unsaved_changes_export"),
                                      QMessageBox::Yes | QMessageBox::No);
        if (reply != QMessageBox::Yes)
        {
            return;
        }
    }
    QString filter = "Cells (*.cells)";
    auto fileName = QFileDialog::getSaveFileName(this, getTranslation(dict, "button_export"),
                                                 options.lastCellSaveLocation, filter, &filter);
    if (fileName.isEmpty())
        return;
    QFileInfo info(fileName);
    options.lastCellSaveLocation = info.path();
    try
    {
        options.saveCells(armoury, fileName);
    }
    catch (const OptionsIoException &e)
    {
        QMessageBox box(QMessageBox::Critical, getTranslation(dict, "error"), e.what.c_str());
        box.exec();
    }
}
