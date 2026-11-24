#include "SettingsManager.h"
#include <sstream>
#include <windows.h>

SettingsManagerWinbasePrivateProfile::SettingsManagerWinbasePrivateProfile(
    const std::string &fileName, const bool autoSaveMissingKeys)
    : fileName_(fileName)
    , autoSaveMissingKeys_(autoSaveMissingKeys)
{}

SettingsManagerWinbasePrivateProfile::~SettingsManagerWinbasePrivateProfile() = default;

bool SettingsManagerWinbasePrivateProfile::ReadBool(const std::string &section,
                                                    const std::string &key,
                                                    const bool &defaultValue)
{
    return ReadAnyType<int>(section, key, defaultValue) != 0;
}

int SettingsManagerWinbasePrivateProfile::ReadInt(const std::string &section,
                                                  const std::string &key,
                                                  const int &defaultValue)
{
    return ReadAnyType<int>(section, key, defaultValue);
}

unsigned int SettingsManagerWinbasePrivateProfile::ReadUInt(const std::string &section,
                                                            const std::string &key,
                                                            const unsigned int &defaultValue)
{
    return ReadAnyType<unsigned int>(section, key, defaultValue);
}

double SettingsManagerWinbasePrivateProfile::ReadDouble(const std::string &section,
                                                        const std::string &key,
                                                        const double &defaultValue)
{
    return ReadAnyType<double>(section, key, defaultValue);
}

std::string SettingsManagerWinbasePrivateProfile::ReadString(const std::string &section,
                                                             const std::string &key,
                                                             const std::string &defaultValue)
{
    char buf[256]{0};
    int bufSize{sizeof(buf)};
    const std::string missingValue = "___" + defaultValue + "___";
    GetPrivateProfileStringA(section.c_str(),
                             key.c_str(),
                             missingValue.c_str(),
                             buf,
                             bufSize,
                             fileName_.c_str());
    std::string readResult{buf};
    if (readResult == missingValue) {
        if (autoSaveMissingKeys_) {
            WriteString(section, key, defaultValue);
        }
        return defaultValue;
    } else {
        return readResult;
    }
}

bool SettingsManagerWinbasePrivateProfile::WriteString(const std::string &section,
                                                       const std::string &key,
                                                       const std::string &value)
{
    return WritePrivateProfileStringA(section.c_str(),
                                      key.c_str(),
                                      value.c_str(),
                                      fileName_.c_str());
}

template<typename T>
T SettingsManagerWinbasePrivateProfile::ReadAnyType(const std::string &section,
                                                    const std::string &key,
                                                    const T &defaultValue)
{
    std::ostringstream defaultValueSs{};
    defaultValueSs << defaultValue;
    const auto readStr = ReadString(section, key, defaultValueSs.str());
    std::istringstream resultSs(readStr);
    T result{};
    if (!(resultSs >> result)) {
        if (autoSaveMissingKeys_) {
            WriteString(section, key, defaultValueSs.str());
        }
        return defaultValue;
    }
    return result;
}

std::unique_ptr<ISettingsManager> SettingsManagerFactory::Create(const std::string &fileName,
                                                                 const bool autoSaveMissingKeys,
                                                                 const IMPL impl)
{
    switch (impl) {
    case IMPL::WINBASE_PRIVATE_PROFILE:
        return std::unique_ptr<ISettingsManager>(
            new SettingsManagerWinbasePrivateProfile(fileName, autoSaveMissingKeys));
    }
    return nullptr;
}
