#ifndef MONO_SOUND_FIX_H
#define MONO_SOUND_FIX_H

#include "../../../../Common/PatchHelpers/IPointerFinder.h"
#include "../../../../Common/PatchHelpers/TrampolineHookBase.h"
#include <dsound.h>
#include <memory>
#include <windows.h>

class MonoSoundFix : public TrampolineHookBase
{
public:
    virtual ~MonoSoundFix() = default;

    bool InjectHooks() override;

    std::string GetName() const override;

protected:
    virtual std::unique_ptr<IPointerFinder> DirectSoundAddress() const = 0;
    virtual std::unique_ptr<IPointerFinder> SoundMixerTableAddress() const = 0;

    virtual std::unique_ptr<IPointerFinder> RedirectJumpHookFunctionAddress() const override;

private:
    struct PlayObject
    {
        std::uint32_t undefined0;
        std::uint8_t *pcmData;
        std::uint32_t dataSize;
        std::uint32_t undefined1;
        std::uint32_t undefined2;
        std::uint32_t undefined3;
        std::uint32_t undefined4;
        std::uint32_t undefined5;
        std::uint32_t undefined6;
        std::uint32_t undefined7;
        std::uint32_t looping;
        std::uint32_t undefined8;
        std::uint32_t undefined9;
        std::uint32_t soundIndex;
        IDirectSoundBuffer *pDsBuffer;
        std::uint32_t undefined10;
        std::int32_t pan;
        std::int32_t volume;
    };

    struct SoundMixer_s
    {
        std::uint32_t undefined0;
        std::uint32_t undefined1;
        std::uint32_t undefined2;
        std::uint32_t undefined3;
        std::uint32_t undefined4;
        tWAVEFORMATEX *format;
    };

    static const bool properFixWithNewSoundBuffer_{true};

    static SoundMixer_s **s_ppSoundMixerTable_;
    static IDirectSound **s_ppDirectSound_;

    typedef int(__fastcall *SndCreateBufferSig)(PlayObject *self);
    static SndCreateBufferSig s_trampolineFunc_;

    static int __fastcall OnCallSnd_CreateBuffer(PlayObject *self);

    static void FixupWaveFormatEx(tWAVEFORMATEX &wfx);
};

class MonoSoundFix_8_0 final : public MonoSoundFix
{
public:
    virtual ~MonoSoundFix_8_0() = default;

protected:
    std::uintptr_t TrampolineBytesCountToSteal() const override;

    std::unique_ptr<IPointerFinder> RedirectJumpOriginalFunctionAddress() const override;
    std::unique_ptr<IPointerFinder> DirectSoundAddress() const override;
    std::unique_ptr<IPointerFinder> SoundMixerTableAddress() const override;

private:
    MonoSoundFix_8_0() = default;

    friend class MonoSoundFixFactory;
};

class MonoSoundFixFactory
{
public:
    static std::unique_ptr<IPatch> Create(const int gameVersion);
};

#endif // MONO_SOUND_FIX_H
