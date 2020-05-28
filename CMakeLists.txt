cmake_minimum_required(VERSION 2.8.5)

project(libADL-demo C CXX)

# Adhere to GNU filesystem layout conventions
include(GNUInstallDirs)

# Lowercase project name for binaries and packaging
string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)

# Linux/other UNIX get a lower-case binary name
set(TARGET_NAME ${PROJECT_NAME_LOWER}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})

# Additional CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)

# Version number in format X.YY.ZZ
set(VER_X 1)
set(VER_YY 00)
set(VER_ZZ 00)
set(VER_FULL "${VER_X}.${VER_YY}.${VER_ZZ}")
set(VER_DATE "11.09.2019")

# Generate version header from the above
configure_file(
		${PROJECT_SOURCE_DIR}/src/demo/version.h.in
		${PROJECT_BINARY_DIR}/src/demo/version.h
)

if (AROS)
	# AROS will provide the SDL dependency via makefile.
elseif (AMIGAOS3)
	add_subdirectory(deps/ADL)
	add_subdirectory(deps/ptplayer)
	include_directories(${ptplayer_INCLUDE_DIRECTORY} ${ADL_INCLUDE_DIRECTORY})
else ()
	find_package(SDL 1.2 REQUIRED)
	find_package(XMP 4.0 REQUIRED)
	include_directories(
			${SDL_INCLUDE_DIR}
			${XMP_INCLUDE_DIR}
	)
endif ()


set(SOURCES
		src/demo/star_bmp.cpp
		src/demo/hex2surface.cpp
		src/demo/demoTimer.cpp
		src/demo/main.cpp)

include_directories(
		${PROJECT_SOURCE_DIR}/src/demo
		${PROJECT_SOURCE_DIR}/src/assets
		${PROJECT_BINARY_DIR}/src/demo/
)


add_executable(${TARGET_NAME} ${SOURCES} ${HEADERS})

if (MORPHOS AND NOT WARPOS)
	target_link_libraries(${TARGET_NAME} ${SDL_LIBRARY} ${XMP_LIBRARY} -ldebug)
elseif (AROS)
	target_link_libraries(${TARGET_NAME} -lSDL -lxmp)
elseif (AMIGAOS3)
	target_link_libraries(${TARGET_NAME} ADL ptplayer)
else ()
	target_link_libraries(${TARGET_NAME} ${SDL_LIBRARY} ${XMP_LIBRARY})
endif ()



set(DEMO_FLAGS_DEBUG "-O0 -g ${68K_CPU} ${68K_FPU}")
set(DEMO_FLAGS_RELEASE "-flto -fwhole-program -O3 -s ${68K_CPU} ${68K_FPU} -ffast-math")

set(CMAKE_C_FLAGS_DEBUG ${DEMO_FLAGS_DEBUG})
set(CMAKE_CXX_FLAGS_DEBUG ${DEMO_FLAGS_DEBUG})
set(CMAKE_EXE_LINKER_FLAGS_DEBUG ${DEMO_FLAGS_DEBUG})

set(CMAKE_C_FLAGS_RELEASE ${DEMO_FLAGS_RELEASE})
set(CMAKE_CXX_FLAGS_RELEASE ${DEMO_FLAGS_RELEASE})
set(CMAKE_EXE_LINKER_FLAGS_RELEASE ${DEMO_FLAGS_RELEASE})


set(INSTALL_DEST ${CMAKE_INSTALL_BINDIR})
install(TARGETS ${TARGET_NAME} DESTINATION ${INSTALL_DEST})


