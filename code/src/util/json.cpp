#include <util/json.hpp>


bool util::json::parameterCheck(const QJsonObject &json, const std::vector<JsonParameter> &params)
{
    for (const auto &param : params)
        if (!json.contains(param.name) || json[param.name].type() != param.type)
            return false;
    return true;
}

Gear::Skill util::json::jsonToSkill(const QJsonValueRef &json)
{
    if (!json.isObject())
        throw std::logic_error("perk is no object");
    auto perk = json.toObject();
    if (!util::json::parameterCheck(perk, perkParameters))
        throw std::logic_error("perk is non conforming");
    return Gear::Skill(perk[JSON_NAME].toString().toStdString(), perk[JSON_VALUE].toInt());
}

std::string util::json::jsonToUniqueSkill(const QJsonValueRef &json, Dictionary &dict)
{
    if (!json.isObject())
        throw std::logic_error("unique effect is no object");
    auto unique = json.toObject();
    if (!util::json::parameterCheck(unique, uniqueEffectsParameters))
        throw std::logic_error("unique effect is non conforming");
    auto description =unique[JSON_DESCRIPTION].toString().toStdString();
    dict.addEntry(description, description);
    return description;
}

int util::json::getMaxValue(const QJsonObject &json)
{
    int max = 0;
    for (const auto &value : json)
    {
        if (value.isDouble())
            max = std::max(max, value.toInt());
        else
            throw std::logic_error("non int resistance value found");
    }
    return max;
}

int util::json::getValueForLevel(const QJsonObject &json, const std::string &level)
{
    QString str = QString::fromStdString(level);
    if (json.contains(str) && json[str].isDouble())
        return json[str].toInt();
    throw std::logic_error("There is no value for level " + level);
}