cmake_minimum_required(VERSION 3.16)

project(TMIDI LANGUAGES CXX RC)

set(TMIDI_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

if (WIN32)
    set(MIDI_PLATFORM_SOURCE "${TMIDI_SOURCE_DIR}/MidiPlatformWin.cpp")
elseif(APPLE)
    set(MIDI_PLATFORM_SOURCE "${TMIDI_SOURCE_DIR}/MidiPlatformMac.cpp")
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

set(TMIDI_SOURCES
    "${TMIDI_SOURCE_DIR}/tmidi.cpp"
    "${TMIDI_SOURCE_DIR}/GdiResourceManager.cpp"
    "${TMIDI_SOURCE_DIR}/MidiDeviceManager.cpp"
    "${TMIDI_SOURCE_DIR}/MidiStateManager.cpp"
    "${TMIDI_SOURCE_DIR}/MidiBackend.cpp"
    "${MIDI_PLATFORM_SOURCE}"
    "${TMIDI_SOURCE_DIR}/Mt32State.cpp"
    "${TMIDI_SOURCE_DIR}/Threading.cpp"
    "${TMIDI_SOURCE_DIR}/SettingsStore.cpp"
    "${TMIDI_SOURCE_DIR}/Playlist.cpp"
)

set(TMIDI_HEADERS
    "${TMIDI_SOURCE_DIR}/tmidi.h"
    "${TMIDI_SOURCE_DIR}/resource.h"
    "${TMIDI_SOURCE_DIR}/winres.h"
)

set(TMIDI_RESOURCES
    "${TMIDI_SOURCE_DIR}/tmidi.rc"
)

if (WIN32)
    add_executable(TMIDI WIN32
        ${TMIDI_SOURCES}
        ${TMIDI_HEADERS}
        ${TMIDI_RESOURCES}
    )

    target_include_directories(TMIDI PRIVATE "${TMIDI_SOURCE_DIR}")
    target_compile_definitions(TMIDI PRIVATE WIN32 _WINDOWS)

    if (MSVC)
        target_compile_definitions(TMIDI PRIVATE _CRT_SECURE_NO_WARNINGS)
    else()
        target_compile_options(TMIDI PRIVATE -Wall -Wextra)
    endif()

    target_link_libraries(TMIDI PRIVATE comctl32 winmm odbc32 odbccp32)

    if (MINGW)
        target_link_options(TMIDI PRIVATE -mwindows -static -static-libgcc -static-libstdc++)
    endif()

    set_target_properties(TMIDI PROPERTIES
        OUTPUT_NAME "NOAMi-MIDI"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
    )
endif()

if (APPLE)
    add_executable(MidiBackendHarness
        "${TMIDI_SOURCE_DIR}/MidiBackendHarness.cpp"
        "${TMIDI_SOURCE_DIR}/MidiBackend.cpp"
        "${MIDI_PLATFORM_SOURCE}"
    )
    target_include_directories(MidiBackendHarness PRIVATE "${TMIDI_SOURCE_DIR}")
    target_compile_features(MidiBackendHarness PRIVATE cxx_std_17)
    target_link_libraries(MidiBackendHarness PRIVATE "-framework CoreMIDI" "-framework CoreFoundation" "-framework CoreAudio")
endif()
