#include "util/Export.hpp"
#include "Dictionary.hpp"
#include "Options.hpp"
#include "util/Translation.hpp"
#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <iostream>
#include <sstream>

void util::exportTextToClipBoard(const std::string& text)
{
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(QString::fromStdString(text));
}

void util::exportTextToFile(const std::string& text, const Dictionary& dict, Options& options)
{
    auto fileName = QFileDialog::getSaveFileName(nullptr, getTranslation(dict, "menu_save"),
                                                 options.lastExportTextSaveLocation);
    if (fileName.isEmpty())
        return;
    QFileInfo info(fileName);
    options.lastExportTextSaveLocation = info.path();
    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly))
    {
        std::stringstream ss;
        ss << "Couldn't open file \"" << fileName.toStdString() << "\"";
        QMessageBox box(QMessageBox::Critical, getTranslation(dict, "error"),
                        QString::fromStdString(ss.str()));
        return;
    }
    file.write(text.c_str());
}

void util::exportOpenWebpage(const std::string& url)
{
    if (!QDesktopServices::openUrl(QUrl(QString::fromStdString(url))))
    {
        std::cout << "failed to open web page " << url << std::endl;
    }
}

void util::addArmoursetExportToContextMenu(QMenu& contextMenu, const Gear::ArmourSet& set,
                                           const Dictionary& dict, Options& options,
                                           const Gear::Armoury& armoury)
{
    auto contextMenuText = contextMenu.addMenu(getTranslation(dict, "loadout_export_text"));
    auto actionTextToClipBoard =
        contextMenuText->addAction(getTranslation(dict, "loadout_export_to_clipboard"));
    QObject::connect(actionTextToClipBoard, &QAction::triggered, [&set, &dict, &armoury](bool) {
        util::exportTextToClipBoard(set.exportToText(dict, armoury));
    });
    auto actionTextToFile =
        contextMenuText->addAction(getTranslation(dict, "loadout_export_to_file"));
    QObject::connect(actionTextToFile, &QAction::triggered,
                     [&set, &dict, &armoury, &options](bool) {
                         util::exportTextToFile(set.exportToText(dict, armoury), dict, options);
                     });

    constexpr auto dauntlessbuilderWebstring = "https://www.dauntless-builder.com/b/";
    auto contextMenuDauntlessbuilder =
        contextMenu.addMenu(getTranslation(dict, "loadout_export_dauntlessbuilder"));
    auto actionDauntlessbuilderToClipBoard =
        contextMenuDauntlessbuilder->addAction(getTranslation(dict, "loadout_export_to_clipboard"));
    QObject::connect(actionDauntlessbuilderToClipBoard, &QAction::triggered,
                     [&set, &armoury, dauntlessbuilderWebstring](bool) {
                         util::exportTextToClipBoard(dauntlessbuilderWebstring +
                                                     set.getHashIds(armoury));
                     });
    auto actionDauntlessbuilderOpenWebpage =
        contextMenuDauntlessbuilder->addAction(getTranslation(dict, "loadout_export_open_webpage"));
    QObject::connect(actionDauntlessbuilderOpenWebpage, &QAction::triggered,
                     [&set, &armoury, dauntlessbuilderWebstring](bool) {
                         util::exportOpenWebpage(dauntlessbuilderWebstring +
                                                 set.getHashIds(armoury));
                     });
}
