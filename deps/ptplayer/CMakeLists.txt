cmake_minimum_required(VERSION 2.8.5)

# Hack to use VASM to compile the assembly code
set(CMAKE_ASM_COMPILER_ENV_VAR ${CMAKE_ASM_COMPILER_ENV_VAR}" ")
FIND_PROGRAM(VASM vasmm68k_mot)
set(CMAKE_ASM_COMPILER ${VASM})
set(CMAKE_ASM_SOURCE_FILE_EXTENSION ".s")
set(CMAKE_ASM_OUTPUT_EXTENSION ".o")
set(CMAKE_ASM_COMPILE_OBJECT "<CMAKE_ASM_COMPILER> <FLAGS> -o <OBJECT> <SOURCE>")
set(CMAKE_ASM_FLAGS "-m68030 -I${CMAKE_PREFIX_PATH}/m68k-amigaos/ndk-include -Fhunk")


project(ptplayer C ASM)

set(
		SOURCES
		PTPlay30B.s
		Sound.c
)

set(
		HEADERS
		PTPlay30B.h
		Sound.h
		asminterface.h
)

include_directories(
		${CMAKE_CURRENT_SOURCE_DIR}
)

set(${PROJECT_NAME}_INCLUDE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})