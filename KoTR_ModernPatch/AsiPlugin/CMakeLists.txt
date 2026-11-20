cmake_minimum_required(VERSION 3.16)
project(KoTR_ModernPatch LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCES
    sources/dllmain.cpp
    sources/GameApp/GameObjects.cpp
    sources/Hooks/AICarFix.cpp
    sources/Hooks/CabinCamFix.cpp
    sources/Hooks/CollisionsFix.cpp
    sources/Hooks/ExtCameraFix.cpp
    sources/Hooks/InfoPanelFix.cpp
    sources/Hooks/DustParticlesFix.cpp
    sources/Hooks/QueueSortingFix.cpp
    sources/Hooks/STrailersPhysFix.cpp
    sources/Hooks/TechSystemFix.cpp
    sources/Utils/CMatrix.cpp
    sources/Utils/ConfigUtils.cpp
    sources/Utils/CVector.cpp
    sources/Utils/ExeUtils.cpp
    sources/Utils/ModUtils/Patterns.cpp)

add_library(KoTR_ModernPatch SHARED ${SOURCES})

set_target_properties(
  KoTR_ModernPatch
  PROPERTIES SUFFIX ".asi"
             PREFIX ""
             OUTPUT_NAME "KoTR_ModernPatch")

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  target_link_options(
    KoTR_ModernPatch PRIVATE $<$<CONFIG:Release>:-static>
    $<$<CONFIG:Release>:-static-libgcc> $<$<CONFIG:Release>:-static-libstdc++>)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  target_compile_options(KoTR_ModernPatch PRIVATE -fms-extensions
                                                  -fms-compatibility)
endif()
