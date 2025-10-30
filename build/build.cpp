#include "build.hpp"
#include <iostream>
#include <sstream>
#include <vector>

static const std::vector<std::string> srcs = {
    "../src/main.cpp",
    "../src/tcp.cpp",
    "../src/bencode.cpp",
};

void buildExec(const std::vector<std::string> srcs, const std::string_view exec) {
    std::ostringstream cmd;
    cmd << CC << " " << COMMON_FLAGS << " " << CFLAGS << " " << "-I../include";
    for (const auto &src : srcs) {
        cmd << " " << src;
    }
    cmd << " -o " << exec(exec);

    std::cout << "CMD: " + cmd.str() + "\n";
    int res = std::system(cmd.str().c_str());

    if (res != 0) {
        std::cerr << "Failed during compilation\n";
        std::exit(1);
    }
}

void runExec(const std::string_view exec) {
    std::ostringstream cmd;
    cmd << exec(exec);

    std::cout << "EXEC: " + cmd.str() + "\n";
    int res = std::system(cmd.str().c_str());

    if (res != 0) {
        std::cerr << "Failed to run exec\n";
        std::exit(1);
    }
}

int main(int argc, char *argv[]) {
    std::string_view cmd = argv[1];
    std::string_view exec = "torrent";

    if (argc == 1 || cmd == "build") {
        buildExec(srcs, exec);
    } else if (cmd == "run") {
        runExec(exec);
    } else {
        std::cerr << "Unknown command\n";
        return 1;
    }

    return 0;
}
