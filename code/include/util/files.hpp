#ifndef DAUNTLESS_ASS_CONFIG_HPP
#define DAUNTLESS_ASS_CONFIG_HPP

#include <QDir>
#include <QStandardPaths>

namespace util
{
namespace files
{

QString pathAppend(const QString &path1, const QString &path2);

void createDirIfNotExist(const QString &path);

const QString DOCUMENTS_LOCATION =
    pathAppend(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
               "ChaosSaber's Armour Set Search");

} // namespace files
} // namespace util

#endif // !DAUNTLESS_ASS_CONFIG_HPP