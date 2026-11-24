#include "MonoSoundFix.h"
#include "../../../../Common/PatchHelpers/PointerFinder.h"
#include "../../../../KoTR_ModernPatch/AsiPlugin/sources/Utils/CPatch.h"

inline IDirectSound **MonoSoundFix::s_ppDirectSound_ = nullptr;
inline MonoSoundFix::SoundMixer_s **MonoSoundFix::s_ppSoundMixerTable_ = nullptr;
inline MonoSoundFix::SndCreateBufferSig MonoSoundFix::s_trampolineFunc_ = nullptr;

std::string MonoSoundFix::GetName() const
{
    return "MonoSoundFix";
}

int __fastcall MonoSoundFix::OnCallSnd_CreateBuffer(PlayObject *self)
{
    SoundMixer_s *soundMixerTable = *s_ppSoundMixerTable_;

    bool createNewStereoBuffer{false};

    // We only want to rewrite true mono PCM buffers into stereo.
    // Everything else (already-stereo, compressed, etc.) is left alone.
    if (soundMixerTable && soundMixerTable[self->soundIndex].format) {
        auto &soundFormat = *(soundMixerTable[self->soundIndex].format);

        // Game developers left cbSize uninitialized. WAVEFORMATEX requires it
        // to be 0; non-zero values can confuse some modern drivers.
        soundFormat.cbSize = 0;

        // Only touch classic mono PCM (8- or 16-bit). Everything else is
        // left to the original code.
        if ((soundFormat.nChannels == 1) && (soundFormat.wFormatTag == WAVE_FORMAT_PCM)
            && ((soundFormat.wBitsPerSample == 8) || (soundFormat.wBitsPerSample == 16))) {
            // Two possible strategies:
            //
            // 1) Proper conversion – create a brand-new stereo buffer whose
            //    size is 2× the original and duplicate every mono sample into
            //    both left and right channels. This preserves pitch and
            //    quality. Requires a valid IDirectSound pointer and may
            //    invalidate any pointers the game still holds to the old
            //    buffer (usually safe if we do it immediately after creation).
            //
            // 2) Cheap hack – just change nChannels to 2 and let the original
            //    data be interpreted as interleaved L/R. This halves the
            //    effective sample rate per channel and sounds worse.
            if (properFixWithNewSoundBuffer_) {
                // Proper conversion
                createNewStereoBuffer = true;
            } else {
                // Cheap hack
                const std::uint32_t newChannelsCount = 2;
                const std::uint32_t oldChannelsCount = soundFormat.nChannels;
                soundFormat.nChannels = newChannelsCount;
                soundFormat.nSamplesPerSec = static_cast<double>(soundFormat.nSamplesPerSec)
                                             * oldChannelsCount / newChannelsCount;
                self->dataSize = self->dataSize - self->dataSize % newChannelsCount;
                FixupWaveFormatEx(soundFormat);
            }
        }
    }

    // Call the original function
    // It will now create a IDirectSoundBuffer self->pDsBuffer
    auto result = s_trampolineFunc_(self);

    // Done for Cheap hack mode, return
    if (!createNewStereoBuffer) {
        return result;
    }

    // ------------------------------------------------------------------
    // Proper conversion
    // Build a stereo buffer with duplicated mono samples
    // ------------------------------------------------------------------
    if (!self->pDsBuffer) {
        return result;
    }

    tWAVEFORMATEX wfxMono{};
    DWORD written{0};
    if (FAILED(self->pDsBuffer->GetFormat(&wfxMono, sizeof(wfxMono), &written))) {
        return result;
    }

    tWAVEFORMATEX wfxStereo{wfxMono};
    wfxStereo.nChannels = 2;
    FixupWaveFormatEx(wfxStereo);

    if (!((wfxStereo.wBitsPerSample == 8) || (wfxStereo.wBitsPerSample == 16))) {
        return result;
    }

    _DSBUFFERDESC desc{};
    desc.dwSize = sizeof(_DSBUFFERDESC);
    desc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY;
    desc.dwBufferBytes = self->dataSize * 2;
    desc.lpwfxFormat = &wfxStereo;

    IDirectSound *pDirectSound = *s_ppDirectSound_;

    IDirectSoundBuffer *newStereoBuffer = nullptr;
    if (FAILED(pDirectSound->CreateSoundBuffer(&desc, &newStereoBuffer, nullptr)))
        return result;

    const int destAudioSegmentsCount = 2;
    LPVOID destAudioBufferPointers[destAudioSegmentsCount]{nullptr};
    DWORD destAudioBufferSizes[destAudioSegmentsCount]{0};

    if (SUCCEEDED(newStereoBuffer->Lock(0,
                                        desc.dwBufferBytes,
                                        &destAudioBufferPointers[0],
                                        &destAudioBufferSizes[0],
                                        &destAudioBufferPointers[1],
                                        &destAudioBufferSizes[1],
                                        0))) {
        const int bytesPerSample = wfxStereo.wBitsPerSample / 8;

        ptrdiff_t srcBytesWritten{0};

        for (int destBufferSegmentNr = 0; destBufferSegmentNr < destAudioSegmentsCount;
             ++destBufferSegmentNr) {
            if (destAudioBufferPointers[destBufferSegmentNr]) {
                // indexes for mono->stereo copy
                // bytesPerSample==1 (stereo  8bit sound) : 0,0,1,1,2,2,3,3,...
                // bytesPerSample==2 (stereo 16bit sound) : 0,1,0,1,2,3,2,3,4,5,4,5,...
                auto dest = reinterpret_cast<std::uint8_t *>(
                    destAudioBufferPointers[destBufferSegmentNr]);
                for (int i = srcBytesWritten;
                     i < destAudioBufferSizes[destBufferSegmentNr] + srcBytesWritten;
                     ++i) {
                    dest[i] = self->pcmData[(i / (2 * bytesPerSample)) * bytesPerSample
                                            + (i % bytesPerSample)];
                }
                srcBytesWritten += destAudioBufferSizes[destBufferSegmentNr];
            }
        }

        newStereoBuffer->Unlock(destAudioBufferPointers[0],
                                destAudioBufferSizes[0],
                                destAudioBufferPointers[1],
                                destAudioBufferSizes[1]);
    } else {
        newStereoBuffer->Release();
        return result;
    }

    struct
    {
        LONG volume{0};
        LONG pan{0};
        DWORD frequency{0};
        DWORD status{0};
    } oldBufferProperties{};

    self->pDsBuffer->GetVolume(&oldBufferProperties.volume);
    self->pDsBuffer->GetPan(&oldBufferProperties.pan);
    self->pDsBuffer->GetFrequency(&oldBufferProperties.frequency);
    self->pDsBuffer->GetStatus(&oldBufferProperties.status);

    newStereoBuffer->SetVolume(oldBufferProperties.volume);
    newStereoBuffer->SetPan(oldBufferProperties.pan);
    newStereoBuffer->SetFrequency(oldBufferProperties.frequency);

    if (oldBufferProperties.status & DSBSTATUS_PLAYING) {
        self->pDsBuffer->Stop();
    }

    self->pDsBuffer->Release();
    self->pDsBuffer = newStereoBuffer;
    if (oldBufferProperties.status & DSBSTATUS_PLAYING) {
        self->pDsBuffer->Play(0,
                              0,
                              (oldBufferProperties.status & DSBSTATUS_LOOPING) ? DSBPLAY_LOOPING
                                                                               : 0);
    }

    return result;
}

bool MonoSoundFix::InjectHooks()
{
    if (TrampolineHookBase::InjectHooks()) {
        s_trampolineFunc_ = reinterpret_cast<MonoSoundFix::SndCreateBufferSig>(TrampolineFunction());

        // this is SoundMixer_s**
        // one of dereferences is still nullptr at InjectHook() time
        // that's normal, game will assign object to pointer much later
        // it's saved as is to dereference when you will needed it in the hook later
        s_ppSoundMixerTable_ = reinterpret_cast<SoundMixer_s **>(
            SoundMixerTableAddress()->GetAddress());

        // this is IDirectSound **
        // one of dereferences is still nullptr at InjectHook() time
        // that's normal, game will assign object to pointer much later
        // it's saved as is to dereference when you will needed it in the hook later
        s_ppDirectSound_ = reinterpret_cast<IDirectSound **>(DirectSoundAddress()->GetAddress());

        return true;
    } else {
        s_trampolineFunc_ = nullptr;
        s_ppSoundMixerTable_ = nullptr;
        s_ppDirectSound_ = nullptr;
        return false;
    }
}

std::unique_ptr<IPointerFinder> MonoSoundFix::RedirectJumpHookFunctionAddress() const
{
    return std::make_unique<PointerFinderConst>(
        reinterpret_cast<std::uintptr_t>(OnCallSnd_CreateBuffer));
}

void MonoSoundFix::FixupWaveFormatEx(tWAVEFORMATEX &wfx)
{
    wfx.nBlockAlign = wfx.nChannels * (wfx.wBitsPerSample / 8);
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
}

std::unique_ptr<IPointerFinder> MonoSoundFix_8_0::RedirectJumpOriginalFunctionAddress() const
{
    // return std::make_unique<PointerFinderConst>(0x006049d0); // 1.3
    // return std::make_unique<PointerFinderConst>(0x00605310); // 8.1
    // return std::make_unique<PointerFinderConst>(0x00604740); // 8.2

    // tested on 1.3 & 8.1 & 8.2
    // first 5 bytes "55 8B EC 6A FF" are masked here for RedirectJump patch reasons
    return std::make_unique<PointerFinderByPattern>(
        "? ? ? ? ? 68 ? ? 64 00 64 A1 00 00 00 00 50 64 89 25 00 00 00 00 83 EC 24 53 8B D9", 0);
}

std::unique_ptr<IPointerFinder> MonoSoundFix_8_0::SoundMixerTableAddress() const
{
    // return std::make_unique<PointerFinderConst>(0x007230c4); // 1.3
    // return std::make_unique<PointerFinderConst>(0x00724164); // 8.1
    // return std::make_unique<PointerFinderConst>(0x00724194); // 8.2

    // tested on 1.3 & 8.1 & 8.2
    int addr{};
    CPatch::GetInt(RedirectJumpOriginalFunctionAddress()->GetAddress() + 0x21, &addr);
    if (addr) {
        return std::make_unique<PointerFinderConst>(addr);
    } else {
        return nullptr;
    }
}

std::unique_ptr<IPointerFinder> MonoSoundFix_8_0::DirectSoundAddress() const
{
    // return std::make_unique<PointerFinderConst>(0x007230f0); // 1.3
    // return std::make_unique<PointerFinderConst>(0x00724190); // 8.1
    // return std::make_unique<PointerFinderConst>(0x007241c0); // 8.2

    // tested on 1.3 & 8.1 & 8.2
    int addr{};
    CPatch::GetInt(RedirectJumpOriginalFunctionAddress()->GetAddress() + 0x4C, &addr);
    if (addr) {
        return std::make_unique<PointerFinderConst>(addr);
    } else {
        return nullptr;
    }
}

std::uintptr_t MonoSoundFix_8_0::TrampolineBytesCountToSteal() const
{
    // tested on 1.3 & 8.1 & 8.2
    return 5;
}

std::unique_ptr<IPatch> MonoSoundFixFactory::Create(const int gameVersion)
{
    switch (gameVersion) {
    case 80:
    case 81:
    case 82:
        return std::unique_ptr<IPatch>(new MonoSoundFix_8_0());
    default:
        return nullptr;
    }
}
