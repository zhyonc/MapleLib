cmake_minimum_required(VERSION 3.25)
project(MapleLib)

# Detours OBJECT library
add_library(detours_obj OBJECT
    Detours/src/detours.cpp
    Detours/src/disasm.cpp
    Detours/src/modules.cpp
)

target_include_directories(detours_obj PUBLIC Detours/src)

# MapleLib static library
add_library(MapleLib STATIC 
    AOBHelper.cpp
    AOBScan.cpp
    ProcessHelper.cpp
    Rosemary.cpp
    StringHelper.cpp
    $<TARGET_OBJECTS:detours_obj>
)
target_include_directories(MapleLib PRIVATE Detours/src)

set_target_properties(MapleLib PROPERTIES
    OUTPUT_NAME_DEBUG MapleLibD
    OUTPUT_NAME_RELEASE MapleLib
)
