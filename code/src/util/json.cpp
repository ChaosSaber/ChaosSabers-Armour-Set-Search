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

Gear::Skill util::json::jsonToSkill(const QJsonObject& json,
                                    std::unordered_map<std::string, size_t>& mapSkillNameToId)
{
    if (!util::json::parameterCheck(json, perkParameters))
        throw std::logic_error("perk is non conforming");
    return Gear::Skill(mapSkillNameToId.at(json[JSON_NAME].toString().toStdString()),
                       json[JSON_VALUE].toInt());
}

std::string jsonToUniqueSkill(const QJsonObject& json, Dictionary& dict)
{
    if (!util::json::parameterCheck(json, uniqueEffectsParameters))
        throw std::logic_error("unique effect is non conforming");
    auto description = json[JSON_DESCRIPTION].toString().toStdString();
    dict.addEntry(description, description);
    return description;
}

std::string jsonToUniqueSkill(const QJsonValueRef& json, Dictionary& dict)
{
    if (!json.isObject())
        throw std::logic_error("unique effect is no object");
    return jsonToUniqueSkill(json.toObject(), dict);
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
    // TODO: exchange with list variant and run some checks oer it?
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

std::vector<QJsonObject> getJsonObjectListForLevel(const QJsonValueRef& jsonRef, int level)
{
    if (!jsonRef.isArray())
        throw std::logic_error("expected perk list to be an array");
    std::vector<QJsonObject> objects;
    for (auto& json : jsonRef.toArray())
    {
        if (!json.isObject())
            throw std::logic_error("Perk is not an object");
        auto& object = json.toObject();
        if (parameterCheck(object, perkParametersFromTo))
        {
            if (object[JSON_PERK_FROM].toInt() <= level && level <= object[JSON_PERK_TO].toInt())
            {
                objects.push_back(object);
            }
        }
        else
        {
            objects.push_back(object);
        }
    }
    return objects;
}

Gear::Skill getSkillFromTo(const QJsonValueRef& jsonRef, int from, int to,
                           std::unordered_map<std::string, size_t>& mapSkillNameToId)
{
    return jsonToSkill(getJsonObjectFromTo(jsonRef, from, to), mapSkillNameToId);
}

std::shared_ptr<std::vector<std::string>> getUniqueSkillsFromJson(const QJsonValueRef& jsonRef,
                                                                  Dictionary& dict, int level)
{
    if (!jsonRef.isArray())
        throw std::logic_error("Unique Skills list is not an array");
    auto skills = std::make_shared<std::vector<std::string>>();
    for (const auto& object : getJsonObjectListForLevel(jsonRef, level))
        skills->push_back(jsonToUniqueSkill(object, dict));
    if (skills->empty())
        throw std::logic_error("No unique skills found");
    return skills;
}

} // namespace json
} // namespace util
