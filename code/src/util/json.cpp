#include "util/json.hpp"
#include <QjsonArray>
#include <sstream>

namespace util
{
namespace json
{

bool util::json::parameterCheck(const QJsonObject& json, const std::vector<JsonParameter>& params)
{
    for (const auto& param : params)
    {
        if (!json.contains(param.name))
            return false;
        if (std::find(param.types.begin(), param.types.end(), json[param.name].type()) ==
            param.types.end())
            return false;
    }
    return true;
}

Gear::Skill util::json::jsonToSkill(const QJsonObject& json)
{
    if (!util::json::parameterCheck(json, perkParameters))
        throw std::logic_error("perk is non conforming");
    return Gear::Skill(json[JSON_NAME].toString().toStdString(), json[JSON_VALUE].toInt());
}

std::string jsonToUniqueSkill(const QJsonValueRef& json, Dictionary& dict)
{
    if (!json.isObject())
        throw std::logic_error("unique effect is no object");
    auto unique = json.toObject();
    if (!util::json::parameterCheck(unique, uniqueEffectsParameters))
        throw std::logic_error("unique effect is non conforming");
    auto description = unique[JSON_DESCRIPTION].toString().toStdString();
    dict.addEntry(description, description);
    return description;
}

int util::json::getMaxValue(const QJsonObject& json)
{
    int max = 0;
    for (const auto& value : json)
    {
        if (value.isDouble())
            max = std::max(max, value.toInt());
        else
            throw std::logic_error("non int resistance value found");
    }
    return max;
}

int util::json::getValueForLevel(const QJsonObject& json, const std::string& level)
{
    QString str = QString::fromStdString(level);
    if (json.contains(str) && json[str].isDouble())
        return json[str].toInt();
    throw std::logic_error("There is no value for level " + level);
}

QJsonObject getJsonObjectFromTo(const QJsonValueRef& jsonRef, int from, int to)
{
    if (!jsonRef.isArray())
        throw std::logic_error("expected perk list to be an array");
    for (auto& json : jsonRef.toArray())
    {
        if (!json.isObject())
            throw std::logic_error("Perk is not an object");
        auto& object = json.toObject();
        if (parameterCheck(object, perkParametersFromTo))
        {
            if (object[JSON_PERK_FROM].toInt() == from && object[JSON_PERK_TO].toInt() == to)
            {
                return object;
            }
        }
        else
        {
            throw std::logic_error("Found Perk without from and to value");
        }
    }
	std::stringstream ss;
    ss << "Found no perk within specified from and to range (" << from << "-" << to;
    throw std::logic_error(ss.str());
}

Gear::Skill getSkillFromTo(const QJsonValueRef& jsonRef, int from, int to)
{
    return jsonToSkill(getJsonObjectFromTo(jsonRef, from, to));
}

std::vector<std::string> getUniqueSkillsFromJson(const QJsonValueRef& jsonRef, Dictionary& dict)
{
    if (!jsonRef.isArray())
        throw std::logic_error("Unique Skills list is not an array");
    std::vector<std::string> skills;
    for (auto& json : jsonRef.toArray())
        skills.push_back(jsonToUniqueSkill(json, dict));
    return skills;
}

} // namespace json
} // namespace util
