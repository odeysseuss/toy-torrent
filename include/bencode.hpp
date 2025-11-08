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
