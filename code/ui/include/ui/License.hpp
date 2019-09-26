#ifndef DAUNTLESS_ASS_UI_LICENSE_HPP
#define DAUNTLESS_ASS_UI_LICENSE_HPP

#include "Dictionary.hpp"
#include <QDialog>

namespace Ui
{
class License;
}

class License : public QDialog
{
    Q_OBJECT
  public:
    License(const Dictionary &dict, QWidget *parent);
    ~License();

  private:
    void quit();
    Ui::License *ui;
};

#endif // !DAUNTLESS_ASS_UI_LICENSE_HPP