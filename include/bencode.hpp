#pragma once

#include <string>
#include <unordered_map>
#include <vector>

enum class BenCodeType {
    INTEGER,
    STRING,
    LIST,
    DICT,
};

class BenCodeVal {
private:
    BenCodeType type;
    union {
        int64_t BenCodeInt;
        std::string BenCodeStr;
        std::vector<BenCodeVal> BenCodeList;
        std::unordered_map<std::string, BenCodeVal> BenCodeDict;
    };

    void cleanup();

public:
    // constructors
    BenCodeVal();
    BenCodeVal(int64_t val);
    BenCodeVal(const std::string val);
    BenCodeVal(std::vector<BenCodeVal> val);
    BenCodeVal(std::unordered_map<std::string, BenCodeVal> val);

    // assignment operators
    BenCodeVal &operator=(const BenCodeVal &other);

    // destructor
    ~BenCodeVal();

    // helper methods to check type
    BenCodeType getType() const {
        return type;
    }
};

class BenCodeDecoder {
private:
    std::string input;
    size_t pos;

    // helper functions
    char peek() const;
    char getChar();
    void consume(char ch);
    std::string readUntil(char del);

    // read input
    int64_t readInt();
    std::string readStr();
    std::vector<BenCodeVal> readList();
    std::unordered_map<std::string, BenCodeVal> readDict();

public:
    // constructor
    BenCodeDecoder(const std::string &input);
    // destructor
    ~BenCodeDecoder();

    // core decoder method
    BenCodeVal decode();
};
