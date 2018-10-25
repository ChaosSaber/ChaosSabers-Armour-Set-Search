#ifndef DAUNTLESS_ASS_UTIL_JSON_HPP
#define DAUNTLESS_ASS_UTIL_JSON_HPP

#include "gear/Skill.hpp"
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonValueRef>
#include <QString>
#include <vector>

#define JSON_NAME "name"
#define JSON_DESCRIPTION "description"
#define JSON_VALUE "value"
#define JSON_PERK_FROM "from"
#define JSON_PERK_TO "to"

namespace util
{
namespace json
{
struct JsonParameter
{
    JsonParameter(QString name, QJsonValue::Type type)
        : JsonParameter(std::move(name), std::vector<QJsonValue::Type>{type})
    {
    }
    JsonParameter(QString name, std::vector<QJsonValue::Type> types)
        : name(std::move(name)), types(types)
    {
    }
    QString name;
    std::vector<QJsonValue::Type> types;
};

bool parameterCheck(const QJsonObject &json, const std::vector<JsonParameter> &params);

Gear::Skill jsonToSkill(const QJsonValueRef &json);
std::string jsonToUniqueSkill(const QJsonValueRef &json, Dictionary &dict);
int getMaxValue(const QJsonObject &json);
int getValueForLevel(const QJsonObject &json, const std::string &level);

#pragma region parameterChecks

const std::vector<util::json::JsonParameter> maelstroemPerkParameters = {
    {JSON_PERK_FROM, QJsonValue::Type::Double}, {JSON_PERK_TO, QJsonValue::Type::Double}};

const std::vector<util::json::JsonParameter> perkParameters = {
    {JSON_NAME, QJsonValue::Type::String}, {JSON_VALUE, QJsonValue::Type::Double}};

const std::vector<util::json::JsonParameter> uniqueEffectsParameters = {
    {JSON_DESCRIPTION, QJsonValue::Type::String}};

#pragma endregion

template <typename T>
void addMaelstromSkill(std::vector<T> &normal, std::vector<T> &heroic, const QJsonValueRef &jsonRef,
                       const T &value)
{
    auto json = jsonRef.toObject();
    if (parameterCheck(json, maelstroemPerkParameters))
    {
        int from = json[JSON_PERK_FROM].toInt();
        int to = json[JSON_PERK_TO].toInt();
        // we need to differentiate between heroic and normal
        if (from == 0 && to == 5)
            normal.push_back(value);
        else if (from == 6 && to == 10)
            heroic.push_back(value);
        else
            throw std::logic_error("non conforming maelstrom perk");
    }
    else
    {
        normal.push_back(value);
        heroic.push_back(value);
    }
}

} // namespace json
} // namespace util

#endif // !DAUNTLESS_ASS_UTIL_JSON_HPP