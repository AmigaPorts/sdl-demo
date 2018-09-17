cmake_minimum_required(VERSION 2.8.5)
project(SDL-demo)

# Adhere to GNU filesystem layout conventions
include(GNUInstallDirs)

# Lowercase project name for binaries and packaging
string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)

# Additional CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

# Version number in format X.YY.ZZ
set(VER_X  1)
set(VER_YY 00)
set(VER_ZZ 00)
set(VER_FULL "${VER_X}.${VER_YY}.${VER_ZZ}")

if(AROS)
    # AROS will provide the SDL dependency via makefile.
else
    find_package(SDL 1.2 REQUIRED)
endif()

set(
    SOURCES
    src/demo/main.cpp
    src/demo/star_bmp.cpp
    src/demo/hex2surface.cpp
    src/demo/timer.cpp
)

include_directories(
    ${SDL_INCLUDE_DIR}
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src/demo
)

# Linux/other UNIX get a lower-case binary name
set(TARGET_NAME ${PROJECT_NAME_LOWER})
add_executable(${TARGET_NAME} ${SOURCES} ${HEADERS})
if(MORPHOS OR AMIGAOS3 AND NOT WARPOS)
    target_link_libraries(${TARGET_NAME} ${SDL_LIBRARY} -ldebug)
else()
    target_link_libraries(${TARGET_NAME} ${SDL_LIBRARY})
endif()

set(INSTALL_DEST ${CMAKE_INSTALL_BINDIR})
install(TARGETS ${TARGET_NAME} DESTINATION ${INSTALL_DEST})


