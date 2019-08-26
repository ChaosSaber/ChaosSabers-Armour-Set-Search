#include "ui/About.hpp"
#include "ui/License.hpp"
#include "ui/Translation.hpp"
#include "ui_About.h"
#include <sstream>

About::About(const Dictionary &dict, QWidget *parent)
    : QDialog(parent), ui(new Ui::About), dict(dict)
{
    ui->setupUi(this);
    setWindowTitle(getTranslation(dict, "menu_about"));
    ui->textEditAbout->setAlignment(Qt::AlignCenter);
    std::stringstream ss;
    ss << "<b><c>Dauntless Armour Set Search<br>" << dict.getTranslationFor("about_version") << " "
       << PROGRAM_VERSION << "</b><br><br>Copyright 2018-2019 ChaosSaber</c>";
    ui->textEditAbout->setText(QString::fromStdString(ss.str()));
    centerText();
    ui->textEditAbout->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    connect(ui->pushButtonClose, &QPushButton::clicked, [this]() { quit(); });
    connect(ui->pushButtonLicense, &QPushButton::clicked, [this]() { showLicense(); });
}

About::~About() { delete ui; }

void About::quit() { delete this; }

void About::showLicense()
{
    auto license = new License(dict, this);
    license->exec();
}

void About::centerText()
{
    QTextCursor cursor = ui->textEditAbout->textCursor();
    QTextBlockFormat textBlockFormat = cursor.blockFormat();
    textBlockFormat.setAlignment(Qt::AlignCenter);
    cursor.mergeBlockFormat(textBlockFormat);
    ui->textEditAbout->setTextCursor(cursor);
}