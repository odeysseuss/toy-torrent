#include "bencode.hpp"

// constructors
BenCodeVal::BenCodeVal() : type(BenCodeType::INTEGER), BenCodeInt(0) {};
BenCodeVal::BenCodeVal(int64_t val) : type(BenCodeType::INTEGER), BenCodeInt(val) {};
BenCodeVal::BenCodeVal(const std::string val) : type(BenCodeType::STRING) {
    new (&BenCodeStr) std::string(val);
}

BenCodeVal::BenCodeVal(std::vector<BenCodeVal> val) : type(BenCodeType::LIST) {
    new (&BenCodeList) std::vector<BenCodeVal>(std::move(val));
}

BenCodeVal::BenCodeVal(std::unordered_map<std::string, BenCodeVal> val) : type(BenCodeType::DICT) {
    new (&BenCodeDict) std::unordered_map<std::string, BenCodeVal>(std::move(val));
}

// assignment operators
BenCodeVal &BenCodeVal::operator=(const BenCodeVal &other) {
    if (this != &other) {
        cleanup();
        switch (other.type) {
        case BenCodeType::INTEGER:
            new (&BenCodeInt) int64_t(other.BenCodeInt);
            break;
        case BenCodeType::STRING:
            new (&BenCodeStr) std::string(other.BenCodeStr);
            break;
        case BenCodeType::LIST:
            new (&BenCodeList) std::vector<BenCodeVal>(other.BenCodeList);
            break;
        case BenCodeType::DICT:
            new (&BenCodeDict) std::unordered_map<std::string, BenCodeVal>(other.BenCodeDict);
            break;
        }
    }
    return *this;
}

// destructor
void BenCodeVal::cleanup() {
    switch (type) {
    case BenCodeType::INTEGER:
        break;
    case BenCodeType::STRING:
        BenCodeStr.~basic_string();
        break;
    case BenCodeType::LIST:
        BenCodeList.~vector();
        break;
    case BenCodeType::DICT:
        BenCodeDict.~unordered_map();
        break;
    }
}

BenCodeVal::~BenCodeVal() {
    cleanup();
}
