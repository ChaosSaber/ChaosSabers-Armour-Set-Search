#include "ArmourSetSearch.hpp"
#include "ui/mainwindow.hpp"
#include <QApplication>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("ChaosSaber's Software");
    QCoreApplication::setOrganizationDomain("https://github.com/ChaosSaber");
    app.setApplicationName("ChaosSaber's Armour Set Search");
    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
