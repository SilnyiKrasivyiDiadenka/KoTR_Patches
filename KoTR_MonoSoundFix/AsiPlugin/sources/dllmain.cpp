#include "dllmain.h"
#include "../../../KoTR_ModernPatch/AsiPlugin/sources/Utils/ExeUtils.h"
#include "Hooks/MonoSoundFix.h"
#include <iostream>
#include <memory>
#include <vector>
#include <windows.h>

namespace {
std::vector<std::unique_ptr<IPatch>> patchHolder;
}

bool printDebugInfo;

void printInfo(const char *text)
{
    if (printDebugInfo)
        std::cout << text << std::endl;
}

void printGameVersion(int version)
{
    if (!printDebugInfo)
        return;

    std::cout << "Detected game version: ";

    if (version) {
        //просто чтобы текст был красивый / just to make the text look nice :)
        std::cout << (version / 10) % 10 << "." << version % 10 << std::endl;
    } else {
        std::cout << "unknown (failed to detect)" << std::endl;
    }
}

void displayConsole()
{
    AllocConsole();
    freopen("conin$", "r", stdin);
    freopen("conout$", "w", stdout);
    freopen("conout$", "w", stderr);
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH: {
        LPCSTR configName = ".\\KoTR_MonoSoundFix.ini";
        int gameVersion = GetGameVersion();

        printDebugInfo = GetPrivateProfileIntA("common", "DisplayConsole", 0, configName);
        if (printDebugInfo)
            displayConsole();

        printInfo("Base functionality from Modern Patch");
        printInfo("https://github.com/aleko2144/KoTR_Modern_Patch");
        printInfo("");
        printInfo("MonoSoundFix v1.0.0");
        printInfo("https://github.com/SilnyiKrasivyiDiadenka/KoTR_Patches");
        printInfo("");
        printGameVersion(gameVersion);

        if (GetPrivateProfileIntA("patches", "MonoSoundFix", 0, configName)) {
            auto monoSoundFix = MonoSoundFixFactory::create(gameVersion);
            if (monoSoundFix && monoSoundFix->InjectHooks()) {
                std::string log = "[PATCH]: injected " + monoSoundFix->GetName() + " patch";
                printInfo(log.c_str());
                patchHolder.push_back(std::move(monoSoundFix));
            }
        }

        break;
    }
    default:
        break;
    }

    return TRUE;
}
