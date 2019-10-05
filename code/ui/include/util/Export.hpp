#pragma once

#include "Dictionary.hpp"
#include "Options.hpp"
#include "gear/ArmourSet.hpp"
#include <QMenu>
#include <QString>
#include <string>

namespace util
{
/**
 * Exports the given text to the clip board.
 * @param text The text to export.
 */
void exportTextToClipBoard(const std::string& text);

/**
 * Exports the given text to a file specifiable by an opening file browser.
 * @param text The text to export.
 * @param dict The Dictionary for the UI Translation.
 * @param options An Options class with the information of the last saving location
 */
void exportTextToFile(const std::string& text, const Dictionary& dict, Options& options);

/**
 * Open the given url in a web browser.
 * @param url The url to open.
 */
void exportOpenWebpage(const std::string& url);

/**
 * Adds export menu entries for armour sets.
 * @param contextMenu The context menu where the entries are needed to added.
 * @param set The Armour set for which the entries needs to be added.
 * @param dict The Dictionary for the UI Translation.
 * @param options An Options class with the information of the last saving location
 * @param armoury The armoury for additional qear informations.
 */
void addArmoursetExportToContextMenu(QMenu& contextMenu, const Gear::ArmourSet& set,
                                     const Dictionary& dict, Options& options, const Gear::Armoury& armoury);
} // namespace util
