#include "bencode.hpp"
#include <iostream>

int main() {
    try {
        // integer
        BenCodeDecoder decoder1("i42e");
        BenCodeVal result1 = decoder1.decode();
        std::cout << "Integer: " << result1.toString() << std::endl;

        // string
        BenCodeDecoder decoder2("4:spam");
        BenCodeVal result2 = decoder2.decode();
        std::cout << "String: " << result2.toString() << std::endl;

        // list
        BenCodeDecoder decoder3("li1ei2e3:fooe");
        BenCodeVal result3 = decoder3.decode();
        std::cout << "List: " << result3.toString() << std::endl;

        // dictionary
        BenCodeDecoder decoder4("d3:bar4:spam3:fooi42ee");
        BenCodeVal result4 = decoder4.decode();
        std::cout << "Dict: " << result4.toString() << std::endl;

    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
