#ifndef POINTER_FINDER_H
#define POINTER_FINDER_H

#include "IPointerFinder.h"
#include <string>

class PointerFinderConst : public IPointerFinder
{
public:
    virtual ~PointerFinderConst() = default;

    PointerFinderConst(const std::uintptr_t address);

    std::uintptr_t GetAddress() override;

private:
    std::uintptr_t address_{};
};

class PointerFinderByPattern : public IPointerFinder
{
public:
    virtual ~PointerFinderByPattern() = default;

    PointerFinderByPattern(const std::string &pattern, const ptrdiff_t offset);

    std::uintptr_t GetAddress() override;

private:
    std::string pattern_{};
    ptrdiff_t offset_{};
};

#endif //POINTER_FINDER_H
