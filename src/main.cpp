#include "bencode.hpp"
#include <iostream>

using std::cout, std::cerr, std::exception;

int main() {
    try {
        // Test 1: Integer
        string init_val = "i42e";
        BDecode decoder1(init_val);
        BVal result1 = decoder1.decode();
        BEncode encoder1;
        string encoded1 = encoder1.encode(result1);

        cout << "Initial Integer: " << init_val << "\n"
                  << "Decoded Integer: " << result1.toString() << "\n"
                  << "Encoded Integer: " << encoded1 << "\n\n";

        // Test 2: String
        init_val = "4:spam";
        BDecode decoder2(init_val);
        BVal result2 = decoder2.decode();
        BEncode encoder2;
        string encoded2 = encoder2.encode(result2);

        cout << "Initial String: " << init_val << "\n"
                  << "Decoded String: " << result2.toString() << "\n"
                  << "Encoded String: " << encoded2 << "\n\n";

        // Test 3: List
        init_val = "li1ei2e3:fooe";
        BDecode decoder3(init_val);
        BVal result3 = decoder3.decode();
        BEncode encoder3;
        string encoded3 = encoder3.encode(result3);

        cout << "Initial List: " << init_val << "\n"
                  << "Decoded List: " << result3.toString() << "\n"
                  << "Encoded List: " << encoded3 << "\n\n";

        // Test 4: Dictionary
        init_val = "d3:bar4:spam3:fooi42ee";
        BDecode decoder4(init_val);
        BVal result4 = decoder4.decode();
        BEncode encoder4;
        string encoded4 = encoder4.encode(result4);

        cout << "Initial Dict: " << init_val << "\n"
                  << "Decoded Dict: " << result4.toString() << "\n"
                  << "Encoded Dict: " << encoded4 << "\n";
    } catch (const exception &e) {
        cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
