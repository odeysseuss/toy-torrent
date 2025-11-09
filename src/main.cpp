#include "bencode.hpp"
#include <iostream>

int main() {
    try {
        // integer
        BDecode decoder1("i42e");
        BVal result1 = decoder1.decode();
        std::cout << "Integer: " << result1.toString() << std::endl;

        // string
        BDecode decoder2("4:spam");
        BVal result2 = decoder2.decode();
        std::cout << "String: " << result2.toString() << std::endl;

        // list
        BDecode decoder3("li1ei2e3:fooe");
        BVal result3 = decoder3.decode();
        std::cout << "List: " << result3.toString() << std::endl;

        // dictionary
        BDecode decoder4("d3:bar4:spam3:fooi42ee");
        BVal result4 = decoder4.decode();
        std::cout << "Dict: " << result4.toString() << std::endl;

    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
