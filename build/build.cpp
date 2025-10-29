#include <iostream>
#include <sstream>
#include <vector>

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

#if defined(COMPILER_CLANG) || defined(COMPILER_GCC)
#    define COMMON_FLAGS "-Wall -Wextra -pedantic "
#    if defined(BUILD_RELEASE)
#        define CFLAGS "-O2 -fstack-protector-strong -D_FORTIFY_SOURCE=2 -march=native"
#    else
#        define CFLAGS "-g3"
#    endif
#elif defined(COMPILER_CL)
#    define COMPILER_FLAGS "/W4 /permissive- "
#    if defined(BUILD_RELEASE)
#        define CFLAGS "/O2 /GS /GL"
#    else
#        define CFLAGS "/Zi /DEBUG"
#    endif
#else
#    error "Only clang, g++ and msvc are supported"
#endif

static const std::vector<std::string> srcs = {
    "../src/main.cpp",
    // "../src/parser.cpp",
};

int main(void) {
    std::string exec = "torrent";
    std::string cc = CC;
    std::string flags = COMMON_FLAGS;
    flags.append(CFLAGS);

    std::ostringstream cmd;
    cmd << cc << " " << flags;
    for (const auto &src : srcs) {
        cmd << " " << src;
    }
    cmd << " -o " << exec;

    std::cout << "CMD: " + cmd.str() + "\n";
    int res = std::system(cmd.str().c_str());

    if (res != 0) {
        std::cerr << "Failed during compilation\n";
    }

    return 0;
}
