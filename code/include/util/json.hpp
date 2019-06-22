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

bool parameterCheck(const QJsonObject& json, const std::vector<JsonParameter>& params);

Gear::Skill jsonToSkill(const QJsonObject& json);
std::string jsonToUniqueSkill(const QJsonValueRef& json, Dictionary& dict);
int getMaxValue(const QJsonObject& json);
int getValueForLevel(const QJsonObject& json, const std::string& level);

#pragma region parameterChecks

const std::vector<util::json::JsonParameter> perkParametersFromTo = {
    {JSON_PERK_FROM, QJsonValue::Type::Double}, {JSON_PERK_TO, QJsonValue::Type::Double}};

const std::vector<util::json::JsonParameter> perkParameters = {
    {JSON_NAME, QJsonValue::Type::String}, {JSON_VALUE, QJsonValue::Type::Double}};

const std::vector<util::json::JsonParameter> uniqueEffectsParameters = {
    {JSON_DESCRIPTION, QJsonValue::Type::String}};

#pragma endregion

Gear::Skill getSkillFromTo(const QJsonValueRef& jsonRef, int from, int to);
std::vector<std::string> getUniqueSkillsFromJson(const QJsonValueRef& jsonRef, Dictionary& dict, int level);

} // namespace json
} // namespace util

#endif // !DAUNTLESS_ASS_UTIL_JSON_HPP