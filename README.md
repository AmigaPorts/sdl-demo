# sdl-demo

In order to compile this project for Amiga, you need to add this to your CMake toolchain file:
```
SET(CMAKE_ASM_SOURCE_FILE_EXTENSION ".s")
set(CMAKE_ASM_OUTPUT_EXTENSION ".o")
set(CMAKE_ASM_COMPILE_OBJECT "<CMAKE_ASM${ASM_DIALECT}_COMPILER> <FLAGS> -o <OBJECT> <SOURCE>")

# Compiler flags
set(CMAKE_ASM_FLAGS "-m68030 -I${tools}/m68k-amigaos/ndk-include -Fhunk")

```
