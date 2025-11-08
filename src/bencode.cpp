#include "bencode.hpp"
#include <stdexcept>

// === Decoder values ===

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

// === Decoder interface ===

// helper functions
char BenCodeDecoder::peek() const {
    if (pos >= input.size())
        return '\0';
    return input[pos];
}

char BenCodeDecoder::getChar() {
    if (pos >= input.size())
        throw std::runtime_error("Unexpected end of input");
    return input[pos++];
}

void BenCodeDecoder::consume(char ch) {
    if (pos >= input.size() || input[pos] != ch)
        throw std::runtime_error("Unexpected input");
    pos++;
}

std::string BenCodeDecoder::readUntil(char delimeter) {
    std::size_t start = pos;
    while (pos < input.size() && input[pos] != delimeter) {
        pos++;
    }
    if (pos >= input.size())
        throw std::runtime_error("Unexpected end of input");

    return input.substr(start, pos - start);
}
