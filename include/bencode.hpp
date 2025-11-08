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

    void copy(const BenCodeVal &other);
    void move(BenCodeVal &&other);
    void cleanup();

public:
    // constructors
    BenCodeVal();
    BenCodeVal(int64_t val);
    BenCodeVal(std::string val);
    BenCodeVal(std::vector<BenCodeVal> val);
    BenCodeVal(std::unordered_map<std::string, BenCodeVal> val);

    // copy constructor
    BenCodeVal(const BenCodeVal &other);
    // move constructor
    BenCodeVal(BenCodeVal &&other) noexcept;

    // assignment operators
    BenCodeVal &operator=(const BenCodeVal &other);
    BenCodeVal &operator=(BenCodeVal &&other) noexcept;

    // destructor
    ~BenCodeVal();

    // helper methods to check type
    BenCodeType getType() const {
        return type;
    }

    // debug helpers
    std::string toString() const;
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

    // decode helper
    BenCodeVal decodeValue();

public:
    // constructor
    BenCodeDecoder(const std::string &input);

    // core decoder method
    BenCodeVal decode();
};
