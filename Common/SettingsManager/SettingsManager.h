#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H

#include "ISettingsManager.h"
#include <memory>

class SettingsManagerWinbasePrivateProfile : public ISettingsManager
{
public:
    SettingsManagerWinbasePrivateProfile(const std::string &fileName, const bool autoSaveMissingKeys);
    
    virtual ~SettingsManagerWinbasePrivateProfile();

    bool ReadBool(const std::string &section,
                  const std::string &key,
                  const bool &defaultValue) override;

    int ReadInt(const std::string &section,
                const std::string &key,
                const int &defaultValue) override;

    unsigned int ReadUInt(const std::string &section,
                          const std::string &key,
                          const unsigned int &defaultValue) override;

    double ReadDouble(const std::string &section,
                      const std::string &key,
                      const double &defaultValue) override;

    std::string ReadString(const std::string &section,
                           const std::string &key,
                           const std::string &defaultValue) override;

private:
    std::string fileName_{};
    bool autoSaveMissingKeys_{};

    bool WriteString(const std::string &section, const std::string &key, const std::string &value);

    template<typename T>
    T ReadAnyType(const std::string &section, const std::string &key, const T &defaultValue);
};

class SettingsManagerFactory
{
public:
    enum class IMPL { WINBASE_PRIVATE_PROFILE };
    static std::unique_ptr<ISettingsManager> Create(const std::string &fileName,
                                                    const bool autoSaveMissingKeys,
                                                    const IMPL impl = IMPL::WINBASE_PRIVATE_PROFILE);
};

#endif // SETTINGS_MANAGER_H
