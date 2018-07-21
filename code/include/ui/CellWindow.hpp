#ifndef DAUNTLESS_ASS_UI_CELLWINDOW_HPP
#define DAUNTLESS_ASS_UI_CELLWINDOW_HPP

#include "Dictionary.hpp"
#include "Options.hpp"
#include "gear/Armoury.hpp"
#include <QDialog>
#include <QGridLayout>
#include <QSpinBox>

namespace Ui
{
class CellWindow;
}

class CellWindow : public QDialog
{
    Q_OBJECT
  public:
    CellWindow(const Gear::Armoury &armoury, Options &options, const Dictionary &dict,
               QWidget *parent = NULL);
    ~CellWindow();

  protected:
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

  private:
    void layoutCells(int coloumns);
    void importCells();
    void exportCells();
    void save();

    Options &options;
    const Dictionary &dict;
    const Gear::Armoury &armoury;
    Ui::CellWindow *ui;
    std::map<QString, QWidget *> widgets;
    std::unordered_map<Gear::Cell, QSpinBox *> cellSpinBoxen;
    int maximumCellWidth = 0;
    QGridLayout *gridLayout;
    bool unsavedChanges = false;
};

#endif // !DAUNTLESS_ASS_UI_CELLWINDOW_HPP
