#include "PointerFinder.h"
#include "../../KoTR_ModernPatch/AsiPlugin/sources/Utils/ModUtils/Patterns.h"

PointerFinderConst::PointerFinderConst(const std::uintptr_t address)
    : address_(address)
{}

std::uintptr_t PointerFinderConst::GetAddress()
{
    return address_;
}

PointerFinderByPattern::PointerFinderByPattern(const std::string &pattern, const ptrdiff_t offset)
    : pattern_(pattern)
    , offset_(offset)
{}

std::uintptr_t PointerFinderByPattern::GetAddress()
{
    try {
        auto patternMatcher = hook::txn::pattern(pattern_);
        if (patternMatcher.size() != 1) {
            return 0;
        }
        auto addr = patternMatcher.get_first(0);
        if (!addr) {
            return 0;
        }
        return reinterpret_cast<std::uintptr_t>(addr) + offset_;
    } catch (const hook::txn_exception &) {
        return 0;
    }
}
