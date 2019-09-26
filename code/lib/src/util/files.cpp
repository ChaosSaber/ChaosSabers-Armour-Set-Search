#include "util/files.hpp"

QString util::files::pathAppend(const QString &path1, const QString &path2)
{
    return QDir::cleanPath(path1 + QDir::separator() + path2);
}

void util::files::createDirIfNotExist(const QString &path)
{
    QDir dir(path);
    if (!dir.exists())
    {
        dir.mkpath(".");
    }
}
