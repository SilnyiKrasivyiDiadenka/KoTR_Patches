#ifndef TRAMPOLINE_HOOK_BASE_H
#define TRAMPOLINE_HOOK_BASE_H

#include "RedirectJumpHookBase.h"
#include <cstdint>

class TrampolineHookBase : public RedirectJumpHookBase
{
public:
    virtual ~TrampolineHookBase();

protected:
    TrampolineHookBase() = default;

    bool InjectHooks() override;
    bool UninstallHooks() override;

    void *TrampolineFunction();

    // 5+ bytes; near relative JMP size (5) for RedirectJump; rounded up to whole instruction boundary
    virtual std::uintptr_t TrampolineBytesCountToSteal() const = 0;

private:
    TrampolineHookBase(const TrampolineHookBase &other) = delete;

    static const int trampolineSizeLimit = 32;
    uint8_t trampoline_[trampolineSizeLimit]{0};

    bool isInstalled_{false};
};

#endif //TRAMPOLINE_HOOK_BASE_H
