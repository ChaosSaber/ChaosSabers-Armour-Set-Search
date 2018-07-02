#include "ui/Translation.hpp"

QString getTranslation(const Dictionary &dict, const std::string &key)
{
    return QString::fromStdString(dict.getTranslationFor(key));
}