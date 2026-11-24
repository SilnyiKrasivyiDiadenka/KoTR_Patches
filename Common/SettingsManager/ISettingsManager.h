#ifndef I_SETTINGS_MANAGER_H
#define I_SETTINGS_MANAGER_H

#include <string>

class ISettingsManager
{
public:
    virtual ~ISettingsManager() = default;

    virtual bool ReadBool(const std::string &section,
                          const std::string &key,
                          const bool &defaultValue)
        = 0;

    virtual int ReadInt(const std::string &section, const std::string &key, const int &defaultValue)
        = 0;

    virtual unsigned int ReadUInt(const std::string &section,
                                  const std::string &key,
                                  const unsigned int &defaultValue)
        = 0;

    virtual double ReadDouble(const std::string &section,
                              const std::string &key,
                              const double &defaultValue)
        = 0;

    virtual std::string ReadString(const std::string &section,
                                   const std::string &key,
                                   const std::string &defaultValue)
        = 0;
};

#endif // I_SETTINGS_MANAGER_H
