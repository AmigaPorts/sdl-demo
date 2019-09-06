cmake_minimum_required(VERSION 2.8.5)
project(SDL-demo)

# Adhere to GNU filesystem layout conventions
include(GNUInstallDirs)

# Lowercase project name for binaries and packaging
string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)

# Linux/other UNIX get a lower-case binary name
set(TARGET_NAME ${PROJECT_NAME_LOWER})

# Additional CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

# Version number in format X.YY.ZZ
set(VER_X  1)
set(VER_YY 00)
set(VER_ZZ 00)
set(VER_FULL "${VER_X}.${VER_YY}.${VER_ZZ}")

if(AROS)
	# AROS will provide the SDL dependency via makefile.
else()
	find_package(SDL 1.2 REQUIRED)
	if(NOT AMIGA)
		find_package(XMP 4.0 REQUIRED)

	endif()
endif()

set(
		SOURCES
		src/demo/star_bmp.cpp
		src/demo/hex2surface.cpp
		src/demo/timer.cpp
		src/demo/main.cpp)

if(AMIGAOS3)
	set(PT_SOURCES deps/ptplayer/Sound.c)
	add_library(ptplayer ${PT_SOURCES})
	include_directories(${PROJECT_SOURCE_DIR}/deps/ptplayer/)
	#target_compile_options(ptplayer PRIVATE -fbaserel32 -msmall-code)
	#target_link_options(ptplayer PRIVATE -fbaserel32 -msmall-code)
	target_link_libraries(ptplayer ${PROJECT_SOURCE_DIR}/deps/ptplayer/PTPlay30B.o)
endif()

include_directories(
		${SDL_INCLUDE_DIR}
		${XMP_INCLUDE_DIR}
		${PROJECT_SOURCE_DIR}/src/demo
)

add_executable(${TARGET_NAME} ${SOURCES} ${HEADERS})
if(MORPHOS AND NOT WARPOS)
	target_link_libraries(${TARGET_NAME} ${SDL_LIBRARY} ${XMP_LIBRARY} -ldebug)
elseif(AROS)
	target_link_libraries(${TARGET_NAME} -lSDL -lxmp)
elseif(AMIGAOS3)
	target_link_libraries(${TARGET_NAME} -lSDL -ldebug ptplayer)
else()
	target_link_libraries(${TARGET_NAME} ${SDL_LIBRARY} ${XMP_LIBRARY})
endif()

set(INSTALL_DEST ${CMAKE_INSTALL_BINDIR})
install(TARGETS ${TARGET_NAME} DESTINATION ${INSTALL_DEST})


