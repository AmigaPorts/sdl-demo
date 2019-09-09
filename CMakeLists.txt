cmake_minimum_required(VERSION 2.8.5)

project(SDL-demo C CXX ASM)

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
elseif(AMIGAOS3)
	# asd
else()
	find_package(SDL 1.2 REQUIRED)
	find_package(XMP 4.0 REQUIRED)
endif()


set(SOURCES
		src/demo/star_bmp.cpp
		src/demo/hex2surface.cpp
		src/demo/demoTimer.cpp
		src/demo/main.cpp)

if(AMIGAOS3)
	set(PT_SOURCES deps/ptplayer/Sound.c deps/ptplayer/PTPlay30B.s)

	set(AMIGA_SOURCES
			deps/amiga/c2p1x1_8_c5_bm.s
			deps/amiga/amiga_timer.c
			deps/amiga/control.c
			deps/amiga/video.c
	)
	add_library(amigavideo ${AMIGA_SOURCES})
	add_library(ptplayer ${PT_SOURCES})
	include_directories(${PROJECT_SOURCE_DIR}/deps/ptplayer/ ${PROJECT_SOURCE_DIR}/deps/amiga/)
else()
	include_directories(
		${SDL_INCLUDE_DIR}
		${XMP_INCLUDE_DIR}
	)
endif()

include_directories(
	${PROJECT_SOURCE_DIR}/src/demo
	${PROJECT_SOURCE_DIR}/src/assets
)

add_executable(${TARGET_NAME} ${SOURCES} ${HEADERS})
if(MORPHOS AND NOT WARPOS)
	target_link_libraries(${TARGET_NAME} ${SDL_LIBRARY} ${XMP_LIBRARY} -ldebug)
elseif(AROS)
	target_link_libraries(${TARGET_NAME} -lSDL -lxmp)
elseif(AMIGAOS3)
	target_link_libraries(${TARGET_NAME} amigavideo ptplayer -lamiga)
else()
	target_link_libraries(${TARGET_NAME} ${SDL_LIBRARY} ${XMP_LIBRARY})
endif()

set(INSTALL_DEST ${CMAKE_INSTALL_BINDIR})
install(TARGETS ${TARGET_NAME} DESTINATION ${INSTALL_DEST})


