#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

enum class BType {
    INTEGER,
    STRING,
    LIST,
    DICT,
};

class BVal {
private:
    std::variant<int64_t, std::string, std::vector<BVal>, std::unordered_map<std::string, BVal>>
        data;

public:
    // constructors
    BVal() : data(int64_t(0)) {};
    BVal(int64_t val) : data(val) {};
    BVal(std::string val) : data(std::move(val)) {};
    BVal(std::vector<BVal> val) : data(std::move(val)) {};
    BVal(std::unordered_map<std::string, BVal> val) : data(std::move(val)) {};

    // helper methods to check type
    BType getType() const {
        return static_cast<BType>(data.index());
    }

    // safe value access
    int64_t asInteger() const;
    const std::string &asString() const;
    const std::vector<BVal> &asList() const;
    const std::unordered_map<std::string, BVal> &asDict() const;

    // for modification
    std::vector<BVal> &asList();
    std::unordered_map<std::string, BVal> &asDict();

    // debug helpers
    std::string toString() const;
};

class BDecode {
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
    std::vector<BVal> readList();
    std::unordered_map<std::string, BVal> readDict();

    // decode helper
    BVal decodeValue();

public:
    // constructor
    BDecode(const std::string &input) : input(input), pos(0) {};

    // core decoder method
    BVal decode();
};

class BEncode {
private:
    // helper functions for different types
    std::string encodeInteger(int64_t value);
    std::string encodeString(const std::string &value);
    std::string encodeList(const std::vector<BVal> &list);
    std::string encodeDict(const std::unordered_map<std::string, BVal> &dict);

public:
    // core encoder method
    std::string encode(const BVal &value);
};
