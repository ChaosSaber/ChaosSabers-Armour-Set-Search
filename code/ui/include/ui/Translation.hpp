#ifndef DAUNTLESS_ASS_UI_TRANSLATION_HPP
#define DAUNTLESS_ASS_UI_TRANSLATION_HPP

#include "Dictionary.hpp"
#include <QString>

QString getTranslation(const Dictionary &dict, const std::string &key);

#endif // !DAUNTLESS_ASS_UI_TRANSLATION_HPP