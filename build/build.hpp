#ifndef BUILD_HPP
#define BUILD_HPP

// === Platform detection ===
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#    define PLATFORM_WIN
#elif defined(__linux__) || defined(__gnu_linux__)
#    define PLATFORM_LINUX
#endif

// === Compiler detection ===
#if defined(__clang__)
#    define COMPILER_CLANG
#    define CC "clang++"
#elif defined(_MSC_VER)
#    define COMPILER_CL
#    define CC "cl"
#elif defined(__GNUC__)
#    define COMPILER_GCC
#    define CC "g++"
#endif

// === Compiler flags ===
#if defined(COMPILER_CLANG) || defined(COMPILER_GCC)
#    define COMMON_FLAGS "-Wall -Wextra -pedantic"
#    if defined(BUILD_RELEASE)
#        define CFLAGS "-O2 -fstack-protector-strong -D_FORTIFY_SOURCE=2 -march=native"
#        define LDFLAGS "-flto=auto", "-Wl,-z,relro", "-pie"
#    else
#        define CFLAGS "-g3"
#        define LDFLAGS "-fsanitize=address,undefined,leak"
#    endif
#elif defined(COMPILER_CL)
#    define COMMON_FLAGS "/W4 /permissive-"
#    if defined(BUILD_RELEASE)
#        define CFLAGS "/O2 /GS /GL"
#        define LDFLAGS "/LTCG /DYNAMICBASE "/NXCOMPAT"
#    else
#        define CFLAGS "/Zi /DEBUG /fsanitize=address /RTC1"
#        define LDFLAGS "/DEBUG /INCREMENTAL"
#    endif
#else
#    error "Only clang, g++ and msvc are supported"
#endif

// Executable extension
#if defined(PLATFORM_LINUX)
#    define exec(_name) ("./" + std::string(_name))
#elif defined(PLATFORM_WIN)
#    define exec(_name) (std::string(_name) + ".exe")
#else
#    error "Unsupported platform"
#endif

#endif
