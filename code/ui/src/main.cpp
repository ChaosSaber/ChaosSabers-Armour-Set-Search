#include "ArmourSetSearch.hpp"
#include "ui/mainwindow.hpp"
#include <QApplication>
#include <iostream>
#include <ArmourSetSearch.hpp>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("ChaosSaber's Software");
    QCoreApplication::setOrganizationDomain("https://github.com/ChaosSaber");
    app.setApplicationName("ChaosSaber's Armour Set Search");
    qRegisterMetaType<ArmourSetSearch::SearchStatistics>("ArmourSetSearch::SearchStatistics");
    //qRegisterMetaType<Gear::ArmourSet>("Gear::ArmourSet");
    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
