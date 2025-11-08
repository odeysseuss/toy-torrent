#include "bencode.hpp"
#include <stdexcept>

// === Decoder values ===

// constructors
BenCodeVal::BenCodeVal() : type(BenCodeType::INTEGER), BenCodeInt(0) {};
BenCodeVal::BenCodeVal(int64_t val) : type(BenCodeType::INTEGER), BenCodeInt(val) {};
BenCodeVal::BenCodeVal(std::string val) : type(BenCodeType::STRING) {
    new (&BenCodeStr) std::string(std::move(val));
}

BenCodeVal::BenCodeVal(std::vector<BenCodeVal> val) : type(BenCodeType::LIST) {
    new (&BenCodeList) std::vector<BenCodeVal>(std::move(val));
}

BenCodeVal::BenCodeVal(std::unordered_map<std::string, BenCodeVal> val) : type(BenCodeType::DICT) {
    new (&BenCodeDict) std::unordered_map<std::string, BenCodeVal>(std::move(val));
}

// copy constructor
void BenCodeVal::copy(const BenCodeVal &other) {
    type = other.type;
    switch (type) {
    case BenCodeType::INTEGER:
        BenCodeInt = other.BenCodeInt;
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
BenCodeVal::BenCodeVal(const BenCodeVal &other) {
    copy(other);
}

// move constructor
void BenCodeVal::move(BenCodeVal &&other) {
    type = other.type;
    switch (type) {
    case BenCodeType::INTEGER:
        BenCodeInt = other.BenCodeInt;
        break;
    case BenCodeType::STRING:
        new (&BenCodeStr) std::string(std::move(other.BenCodeStr));
        break;
    case BenCodeType::LIST:
        new (&BenCodeList) std::vector<BenCodeVal>(std::move(other.BenCodeList));
        break;
    case BenCodeType::DICT:
        new (&BenCodeDict)
            std::unordered_map<std::string, BenCodeVal>(std::move(other.BenCodeDict));
        break;
    }
}

BenCodeVal::BenCodeVal(BenCodeVal &&other) noexcept {
    move(std::move(other));
}

// assignment operators
BenCodeVal &BenCodeVal::operator=(const BenCodeVal &other) {
    if (this != &other) {
        cleanup();
        copy(other);
    }
    return *this;
}

BenCodeVal &BenCodeVal::operator=(BenCodeVal &&other) noexcept {
    if (this != &other) {
        cleanup();
        move(std::move(other));
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

// constructor
BenCodeDecoder::BenCodeDecoder(const std::string &input) : input(input), pos(0) {};

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

// read input
int64_t BenCodeDecoder::readInt() {
    consume('i');
    std::string numStr = readUntil('e');
    consume('e');

    try {
        return std::stoll(numStr);
    } catch (const std::exception &e) {
        throw std::runtime_error("Invalid Integer format");
    }
}

std::string BenCodeDecoder::readStr() {
    std::size_t length;
    std::string lengthStr = readUntil(':');

    try {
        length = std::stoul(lengthStr);
    } catch (const std::exception &e) {
        throw std::runtime_error("Invalid String format");
    }

    consume(':');

    if (pos + length > input.size()) {
        throw std::runtime_error("Unexpected end of input");
    }

    std::string res = input.substr(pos, length);
    pos += length;
    return res;
}

std::vector<BenCodeVal> BenCodeDecoder::readList() {
    consume('l');
    std::vector<BenCodeVal> list;

    while (peek() != 'e') {
        list.push_back(decodeValue());
    }

    consume('e');
    return list;
}

std::unordered_map<std::string, BenCodeVal> BenCodeDecoder::readDict() {
    consume('d');
    std::unordered_map<std::string, BenCodeVal> dict;

    while (peek() != 'e') {
        std::string key = readStr();
        BenCodeVal val = decodeValue();
        dict.emplace(std::move(key), std::move(val));
    }

    consume('e');
    return dict;
}

BenCodeVal BenCodeDecoder::decodeValue() {
    char ch = peek();

    switch (ch) {
    case 'i':
        return BenCodeVal(readInt());
    case 'l':
        return BenCodeVal(readList());
    case 'd':
        return BenCodeVal(readDict());
    default:
        if (isdigit(ch)) {
            return BenCodeVal(readStr());
        } else {
            throw std::runtime_error("Unexpected char in bencode String");
        }
    }
}

// core decoder method
BenCodeVal BenCodeDecoder::decode() {
    BenCodeVal res = decodeValue();
    if (pos != input.size()) {
        throw std::runtime_error("Extra data after bencoded value");
    }

    return res;
}

// debug helpers
std::string BenCodeVal::toString() const {
    switch (type) {
    case BenCodeType::INTEGER:
        return std::to_string(BenCodeInt);
    case BenCodeType::STRING:
        return "\"" + BenCodeStr + "\"";
    case BenCodeType::LIST: {
        std::string result = "[";
        for (size_t i = 0; i < BenCodeList.size(); ++i) {
            if (i > 0)
                result += ", ";
            result += BenCodeList[i].toString();
        }
        result += "]";
        return result;
    }
    case BenCodeType::DICT: {
        std::string result = "{";
        bool first = true;
        for (const auto &pair : BenCodeDict) {
            if (!first)
                result += ", ";
            result += "\"" + pair.first + "\": " + pair.second.toString();
            first = false;
        }
        result += "}";
        return result;
    }
    }
    return "unknown";
}
