#ifndef LOD_FIX_H
#define LOD_FIX_H

#include "../../../../Common/PatchHelpers/IPatch.h"
#include "../../../../Common/PatchHelpers/TrampolineHookBase.h"
#include <memory>

class LodFixRedirectDrawSlot : public TrampolineHookBase
{
public:
    virtual ~LodFixRedirectDrawSlot() = default;

    bool InjectHooks() override;
    std::string GetName() const override;

protected:
    virtual std::unique_ptr<IPointerFinder> RedirectJumpHookFunctionAddress() const override;

private:
    struct SceneNode
    {
        std::uint32_t undefined0;
        std::uint32_t undefined1;
        std::uint32_t undefined2;
        std::uint32_t undefined3;
        std::uint32_t kind;
        std::uint32_t undefined5;
        std::uint32_t undefined6;
        std::uint32_t undefined7;
        std::uint32_t undefined8;
        std::uint32_t undefined9;
        std::uint32_t undefined10;
        std::uint32_t undefined11;
        std::uint32_t undefined12;
        std::uint32_t lodArraySize;
        std::uint32_t lodDistanceCalcMode;

        struct lodDistanceCalcMode
        {
            enum : std::uint32_t {
                alongTheViewAxis = 0,
                projectedSize = 1,
                distanceToAPlane = 2,
                five = 5
            };
        };

        struct nodeKind
        {
            enum : std::uint32_t { lod = 8 };
        };
    };

    struct SceneView
    {
        std::uint32_t undefined0;
    };

    typedef void(__thiscall *DrawSlotSig)(SceneNode *self,
                                          std::uint32_t lodSlot,
                                          std::uint32_t flags,
                                          SceneView *sceneView);

    static DrawSlotSig s_DrawSlotTrampoline_;

    static void __thiscall OnCallDrawSlot(SceneNode *self,
                                          std::uint32_t lodSlot,
                                          std::uint32_t flags,
                                          SceneView *sceneView);
};

class LodFix_8_0 final : public LodFixRedirectDrawSlot
{
public:
    virtual ~LodFix_8_0() = default;

protected:
    std::unique_ptr<IPointerFinder> RedirectJumpOriginalFunctionAddress() const override;

    std::uintptr_t TrampolineBytesCountToSteal() const override;

private:
    LodFix_8_0() = default;

    friend class LodFixFactory;
};

class LodFixFactory
{
public:
    static std::unique_ptr<IPatch> Create(const int gameVersion);
};

#endif // LOD_FIX_H
