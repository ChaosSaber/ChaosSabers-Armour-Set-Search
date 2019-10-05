#include "ui/License.hpp"
#include "util/Translation.hpp"
#include "ui_License.h"
#include <QFile>
#include <iostream>

License::License(const Dictionary &dict, QWidget *parent) : QDialog(parent), ui(new Ui::License)
{
    ui->setupUi(this);
    setWindowTitle(getTranslation(dict, "menu_license"));
    QFile licenseFile("LICENSE");
    if (!licenseFile.open(QIODevice::ReadOnly))
    {
        // leave presets
        std::cout << "Couldn't open License file";
        return;
    }
    ui->textEditLicense->setText(licenseFile.readAll());
    connect(ui->pushButtonClose, &QPushButton::clicked, [this]() { quit(); });
}
License::~License() { delete ui; }

void License::quit() { delete this; }