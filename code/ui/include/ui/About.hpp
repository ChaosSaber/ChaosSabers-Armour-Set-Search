#ifndef DAUNTLESS_ASS_UI_ABOUT_HPP
#define DAUNTLESS_ASS_UI_ABOUT_HPP

#include "Dictionary.hpp"
#include <QDialog>

namespace Ui
{
class About;
}

class About : public QDialog
{
    Q_OBJECT
  public:
    About(const Dictionary &dict, QWidget *parent);
    ~About();

  private:
    void quit();
    void showLicense();
    void centerText();

    const Dictionary &dict;
    Ui::About *ui;
};

#endif // !DAUNTLESS_ASS_UI_ABOUT_HPP