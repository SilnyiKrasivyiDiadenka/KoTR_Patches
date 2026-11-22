#ifndef I_POINTER_FINDER_H
#define I_POINTER_FINDER_H

#include <cstdint>

class IPointerFinder
{
public:
    virtual ~IPointerFinder() = default;

    virtual std::uintptr_t GetAddress() = 0;

    virtual void *GetPointer()
    {
        auto addr = GetAddress();
        if (!addr) {
            return nullptr;
        } else {
            return reinterpret_cast<void *>(addr);
        }
    }
};

#endif // I_POINTER_FINDER_H
