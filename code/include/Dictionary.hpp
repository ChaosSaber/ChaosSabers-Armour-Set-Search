#ifndef DAUNTLESS_ASS_DICTIONARY_HPP
#define DAUNTLESS_ASS_DICTIONARY_HPP

#include <string>
#include <unordered_map>

class Dictionary
{
  public:
    Dictionary();
    const std::string &getTranslationFor(const std::string &key) const;
    std::vector<std::string> getAvaiableLanguages() const;
    void loadLanguage(const std::string &language);

  private:
    std::string exchangeSpecialCharacter(std::string str);
    std::unordered_map<std::string, std::string> dict;
    std::unordered_map<std::string, int> languages;
};

#pragma region constants

#pragma endregion

#endif // !DAUNTLESS_ASS_DICTIONARY_HPP