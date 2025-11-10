cmake_minimum_required(VERSION 3.16)

project(TMIDI LANGUAGES CXX RC)

set(TMIDI_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

set(TMIDI_SOURCES
    "${TMIDI_SOURCE_DIR}/tmidi.cpp"
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

if (WIN32)
    target_link_libraries(TMIDI PRIVATE comctl32 winmm odbc32 odbccp32)
endif()

if (MINGW)
    target_link_options(TMIDI PRIVATE -mwindows -static -static-libgcc -static-libstdc++)
endif()

set_target_properties(TMIDI PROPERTIES
    OUTPUT_NAME "NOAMi-MIDI"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
