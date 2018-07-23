#include "ArmourSetSearch.hpp"
#include "ui/mainwindow.hpp"
#include <QApplication>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
