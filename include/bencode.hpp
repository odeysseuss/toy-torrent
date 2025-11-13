#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

using std::string, std::vector, std::unordered_map, std::variant;

enum class BType {
    INTEGER,
    STRING,
    LIST,
    DICT,
};

class BVal {
private:
    variant<int64_t, string, vector<BVal>, unordered_map<string, BVal>> data;

public:
    // constructors
    BVal() : data(int64_t(0)) {};
    BVal(int64_t val) : data(val) {};
    BVal(string val) : data(std::move(val)) {};
    BVal(vector<BVal> val) : data(std::move(val)) {};
    BVal(unordered_map<string, BVal> val) : data(std::move(val)) {};

    // helper methods to check type
    BType getType() const {
        return static_cast<BType>(data.index());
    }

    // safe value access
    int64_t asInteger() const;
    const string &asString() const;
    const vector<BVal> &asList() const;
    const unordered_map<string, BVal> &asDict() const;

    // for modification
    vector<BVal> &asList();
    unordered_map<string, BVal> &asDict();

    // debug helpers
    string toString() const;
};

class BDecode {
private:
    string input;
    size_t pos;

    // helper functions
    char peek() const;
    char getChar();
    void consume(char ch);
    string readUntil(char del);

    // read input
    int64_t readInt();
    string readStr();
    vector<BVal> readList();
    unordered_map<string, BVal> readDict();

    // decode helper
    BVal decodeValue();

public:
    // constructor
    BDecode(const string &input) : input(input), pos(0) {};

    // core decoder method
    BVal decode();
};

class BEncode {
private:
    // helper functions for different types
    string encodeInteger(int64_t value);
    string encodeString(const string &value);
    string encodeList(const vector<BVal> &list);
    string encodeDict(const unordered_map<string, BVal> &dict);

public:
    // core encoder method
    string encode(const BVal &value);
};
