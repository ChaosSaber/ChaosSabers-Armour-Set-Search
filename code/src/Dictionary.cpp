#include "Dictionary.hpp"
#include "util/string.hpp"
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <fstream>
#include <iostream>

const QString LANGUAGE = "Language";
const QString COLOUMN = "Coloumn";

Dictionary::Dictionary()
{
    QFile config(QString::fromStdString("language/languages.json"));
    if (!config.open(QIODevice::ReadOnly))
    {
        // leave presets
        std::cout << "Couldn't open languages file";
        return;
    }
    QByteArray configData = config.readAll();
    QJsonDocument jsonDoc(QJsonDocument::fromJson(configData));

    for (const auto &language : jsonDoc.array())
    {
        if (language.isObject())
        {
            auto json = language.toObject();
            if (json.contains(LANGUAGE) && json[LANGUAGE].isString() && json.contains(COLOUMN) &&
                json[COLOUMN].isDouble())
            {
                languages[json[LANGUAGE].toString().toStdString()] = json[COLOUMN].toInt();
            }
            else
            {
                std::cout << "Couldn't load language" << std::endl;
            }
        }
        else
        {
            std::cout << "Couldn't load language" << std::endl;
        }
    }
}

const std::string &Dictionary::getTranslationFor(const std::string &key) const
{
    if (dict.count(key) > 0)
        return dict.at(key);
    std::cout << "Couldn't find key " << key << std::endl;
    return key;
}

std::vector<std::string> Dictionary::getAvaiableLanguages() const
{
    std::vector<std::string> ret;
    for (const auto &language : languages)
        ret.push_back(language.first);
    return ret;
}

void Dictionary::loadLanguage(const std::string &language)
{
    QDir directory("language");
    QStringList files = directory.entryList(QStringList() << "*.txt", QDir::Files);
    for (const auto &file : files)
    {
        auto path = QDir::cleanPath(directory.path() + QDir::separator() + file).toStdString();
        std::ifstream fstream(path);
        if (!fstream.is_open())
        {
            // TODO: errorhandling
            std::cout << "couldn't open file " << file.toStdString() << std::endl;
            continue;
        }
        std::string line;
        while (std::getline(fstream, line))
        {
            if (util::string::isComment(line))
                continue;
            auto tmp = util::string::split(line, ';');
            if (tmp.size() == 1)
                continue;
            int coloumn = languages[language];
            if (tmp.size() <= coloumn || tmp[coloumn].empty())
                coloumn = 1; // load English as default
            if (tmp[coloumn].empty() || tmp[0].empty())
                continue;
            dict[util::string::toLower(tmp[0])] = tmp[coloumn];
        }
    }
}