#include "LodFix.h"
#include "../../../../Common/PatchHelpers/PointerFinder.h"

inline LodFixRedirectDrawSlot::DrawSlotSig LodFixRedirectDrawSlot::s_DrawSlotTrampoline_ = nullptr;

std::string LodFixRedirectDrawSlot::GetName() const
{
    return "LodFix";
}

void __thiscall LodFixRedirectDrawSlot::OnCallDrawSlot(SceneNode *self,
                                                       std::uint32_t lodSlot,
                                                       std::uint32_t flags,
                                                       SceneView *sceneView)
{
    if (s_DrawSlotTrampoline_) {
        switch (self->kind) {
        case SceneNode::nodeKind::lod:
            switch (self->lodDistanceCalcMode) {
            case SceneNode::lodDistanceCalcMode::projectedSize:
                lodSlot = 0;
                break;
            case SceneNode::lodDistanceCalcMode::alongTheViewAxis:
            case SceneNode::lodDistanceCalcMode::distanceToAPlane:
                if (self->lodArraySize > 0) {
                    lodSlot = self->lodArraySize - 1;
                }
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
        s_DrawSlotTrampoline_(self, lodSlot, flags, sceneView);
    }
}

bool LodFixRedirectDrawSlot::InjectHooks()
{
    if (TrampolineHookBase::InjectHooks()) {
        s_DrawSlotTrampoline_ = reinterpret_cast<LodFixRedirectDrawSlot::DrawSlotSig>(
            TrampolineFunction());
        return true;
    } else {
        s_DrawSlotTrampoline_ = nullptr;
        return false;
    }
}

std::unique_ptr<IPointerFinder> LodFixRedirectDrawSlot::RedirectJumpHookFunctionAddress() const
{
    return std::make_unique<PointerFinderConst>(reinterpret_cast<std::uintptr_t>(OnCallDrawSlot));
}

std::unique_ptr<IPointerFinder> LodFix_8_0::RedirectJumpOriginalFunctionAddress() const
{
    // return std::make_unique<PointerFinderConst>(005f16f0); // 1.3
    // return std::make_unique<PointerFinderConst>(005f2080); // 8.1
    // return std::make_unique<PointerFinderConst>(005f2040); // 8.2

    // tested on 1.3 & 8.1 & 8.2
    // first 5 bytes "8B 44 24 04 8B" are masked here for RedirectJump patch reasons
    return std::make_unique<PointerFinderByPattern>("? ? ? ? ? 51 34 53 56", 0);
}

std::uintptr_t LodFix_8_0::TrampolineBytesCountToSteal() const
{
    // tested on 1.3 & 8.1 & 8.2
    return 7;
}

std::unique_ptr<IPatch> LodFixFactory::Create(const int gameVersion)
{
    switch (gameVersion) {
    case 80:
    case 81:
    case 82:
        return std::unique_ptr<IPatch>(new LodFix_8_0());
    default:
        return nullptr;
    }
}
