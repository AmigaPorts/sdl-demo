cmake_minimum_required(VERSION 2.8.5)

project(ptplayer ASM C)

if (AMIGAOS3)
	enable_language(ASM_VASM)
	set(
			SOURCES
			PTPlay30B.asm
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
else()

endif()
