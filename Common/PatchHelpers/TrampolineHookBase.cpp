#include "TrampolineHookBase.h"
#include "../../KoTR_ModernPatch/AsiPlugin/sources/Utils/CPatch.h"

bool TrampolineHookBase::InjectHooks()
{
    if (isInstalled_) {
        return false;
    }

    if ((TrampolineBytesCountToSteal() < redirectJumpInstructionSize)
        || (TrampolineBytesCountToSteal() + redirectJumpInstructionSize > trampolineSizeLimit)) {
        return false;
    }

    void *original = RedirectJumpOriginalFunctionAddress()->GetPointer();
    if (!original) {
        return false;
    }

    CPatch::SetBytes(reinterpret_cast<std::uintptr_t>(trampoline_),
                     original,
                     TrampolineBytesCountToSteal());

    CPatch::RedirectJump(reinterpret_cast<std::uintptr_t>(trampoline_)
                             + TrampolineBytesCountToSteal(),
                         reinterpret_cast<void *>(reinterpret_cast<std::uintptr_t>(original)
                                                  + TrampolineBytesCountToSteal()));

    DWORD lpflOldProtect{};
    VirtualProtect(trampoline_,
                   TrampolineBytesCountToSteal() + redirectJumpInstructionSize,
                   PAGE_EXECUTE_READWRITE,
                   &lpflOldProtect);

    if (!RedirectJumpHookBase::InjectHooks()) {
        return false;
    }

    isInstalled_ = true;

    return true;
}

bool TrampolineHookBase::UninstallHooks()
{
    if (isInstalled_ && RedirectJumpHookBase::UninstallHooks()) {
        isInstalled_ = false;
        return true;
    } else {
        return false;
    }
}

void *TrampolineHookBase::TrampolineFunction()
{
    if (isInstalled_) {
        return trampoline_;
    } else {
        return nullptr;
    }
}

TrampolineHookBase::~TrampolineHookBase()
{
    TrampolineHookBase::UninstallHooks();
}
