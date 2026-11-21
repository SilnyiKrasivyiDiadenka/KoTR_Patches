#ifndef REDIRECT_JUMP_HOOK_BASE_H
#define REDIRECT_JUMP_HOOK_BASE_H

#include "IPointerFinder.h"
#include <cstdint>
#include <memory>

class RedirectJumpHookBase
{
public:
    virtual ~RedirectJumpHookBase();

protected:
    RedirectJumpHookBase() = default;

    static const std::uintptr_t redirectJumpInstructionSize = 5;

    virtual bool Inject();
    virtual bool Uninstall();

    virtual std::unique_ptr<IPointerFinder> RedirectJumpOriginalFunctionAddress() const = 0;
    virtual std::unique_ptr<IPointerFinder> RedirectJumpHookFunctionAddress() const = 0;

private:
    RedirectJumpHookBase(const RedirectJumpHookBase &other) = delete;

    uint8_t backupBytes_[redirectJumpInstructionSize]{0};

    void *original_{nullptr};
    bool isInstalled_{false};
};

#endif // REDIRECT_JUMP_HOOK_BASE_H
