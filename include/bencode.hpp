#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

enum class BenCodeType {
    INTEGER,
    STRING,
    LIST,
    DICT,
};

class BenCodeVal {
private:
    std::variant<int64_t,
                 std::string,
                 std::vector<BenCodeVal>,
                 std::unordered_map<std::string, BenCodeVal>>
        data;

public:
    // constructors
    BenCodeVal() : data(int64_t(0)) {};
    BenCodeVal(int64_t val) : data(val) {};
    BenCodeVal(std::string val) : data(std::move(val)) {};
    BenCodeVal(std::vector<BenCodeVal> val) : data(std::move(val)) {};
    BenCodeVal(std::unordered_map<std::string, BenCodeVal> val) : data(std::move(val)) {};

    // helper methods to check type
    BenCodeType getType() const {
        return static_cast<BenCodeType>(data.index());
    }

    // safe value access
    int64_t asInteger() const;
    const std::string &asString() const;
    const std::vector<BenCodeVal> &asList() const;
    const std::unordered_map<std::string, BenCodeVal> &asDict() const;

    // for modification
    std::vector<BenCodeVal> &asList();
    std::unordered_map<std::string, BenCodeVal> &asDict();

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
    BenCodeDecoder(const std::string &input) : input(input), pos(0) {};

    // core decoder method
    BenCodeVal decode();
};
