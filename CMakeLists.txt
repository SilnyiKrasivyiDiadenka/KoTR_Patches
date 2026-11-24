cmake_minimum_required(VERSION 3.16)
project(KoTR_Patches LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported)
if(ipo_supported)
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
endif()

add_library(
  KoTR_ModernPatch SHARED
  KoTR_ModernPatch/AsiPlugin/sources/dllmain.cpp
  KoTR_ModernPatch/AsiPlugin/sources/GameApp/GameObjects.cpp
  KoTR_ModernPatch/AsiPlugin/sources/Hooks/AICarFix.cpp
  KoTR_ModernPatch/AsiPlugin/sources/Hooks/CabinCamFix.cpp
  KoTR_ModernPatch/AsiPlugin/sources/Hooks/CollisionsFix.cpp
  KoTR_ModernPatch/AsiPlugin/sources/Hooks/ExtCameraFix.cpp
  KoTR_ModernPatch/AsiPlugin/sources/Hooks/InfoPanelFix.cpp
  KoTR_ModernPatch/AsiPlugin/sources/Hooks/DustParticlesFix.cpp
  KoTR_ModernPatch/AsiPlugin/sources/Hooks/QueueSortingFix.cpp
  KoTR_ModernPatch/AsiPlugin/sources/Hooks/STrailersPhysFix.cpp
  KoTR_ModernPatch/AsiPlugin/sources/Hooks/TechSystemFix.cpp
  KoTR_ModernPatch/AsiPlugin/sources/Utils/CMatrix.cpp
  KoTR_ModernPatch/AsiPlugin/sources/Utils/ConfigUtils.cpp
  KoTR_ModernPatch/AsiPlugin/sources/Utils/CVector.cpp
  KoTR_ModernPatch/AsiPlugin/sources/Utils/ExeUtils.cpp
  KoTR_ModernPatch/AsiPlugin/sources/Utils/ModUtils/Patterns.cpp)

add_library(
  KoTR_MonoSoundFix SHARED
  KoTR_MonoSoundFix/AsiPlugin/sources/dllmain.cpp
  KoTR_ModernPatch/AsiPlugin/sources/Utils/ExeUtils.cpp
  KoTR_ModernPatch/AsiPlugin/sources/Utils/ModUtils/Patterns.cpp
  Common/PatchHelpers/PointerFinder.cpp
  Common/PatchHelpers/RedirectJumpHookBase.cpp
  Common/PatchHelpers/TrampolineHookBase.cpp
  Common/SettingsManager/SettingsManager.cpp
  KoTR_MonoSoundFix/AsiPlugin/sources/Hooks/MonoSoundFix.cpp)

set_target_properties(
  KoTR_ModernPatch
  PROPERTIES SUFFIX ".asi"
             PREFIX ""
             OUTPUT_NAME "KoTR_ModernPatch")

set_target_properties(
  KoTR_MonoSoundFix
  PROPERTIES SUFFIX ".asi"
             PREFIX ""
             OUTPUT_NAME "KoTR_MonoSoundFix")

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  target_link_options(
    KoTR_ModernPatch PRIVATE $<$<CONFIG:Release>:-static>
    $<$<CONFIG:Release>:-static-libgcc> $<$<CONFIG:Release>:-static-libstdc++>)

  target_link_options(
    KoTR_MonoSoundFix PRIVATE $<$<CONFIG:Release>:-static>
    $<$<CONFIG:Release>:-static-libgcc> $<$<CONFIG:Release>:-static-libstdc++>)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  target_compile_options(KoTR_ModernPatch PRIVATE -fms-extensions
                                                  -fms-compatibility)

  target_compile_options(KoTR_MonoSoundFix PRIVATE -fms-extensions
                                                   -fms-compatibility)
endif()
