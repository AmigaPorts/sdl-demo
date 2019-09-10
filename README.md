# SDL test demo

## About

This project was made to to test SDL on the different Amiga platforms, it has since evolved into something else
and the purpose is now to test an alternative to SDL that removes the layer of SDL and talks directly with Amiga native
components.

## Building

In order to compile this project for Amiga, you need to add this to your CMake toolchain file:
```
SET(CMAKE_ASM_SOURCE_FILE_EXTENSION ".s")
set(CMAKE_ASM_OUTPUT_EXTENSION ".o")
set(CMAKE_ASM_COMPILE_OBJECT "<CMAKE_ASM${ASM_DIALECT}_COMPILER> <FLAGS> -o <OBJECT> <SOURCE>")

# Compiler flags
set(CMAKE_ASM_FLAGS "-m68030 -I${tools}/m68k-amigaos/ndk-include -Fhunk")

```
