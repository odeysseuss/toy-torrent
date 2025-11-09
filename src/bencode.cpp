#include "bencode.hpp"
#include <stdexcept>

// === BenCodeVal Implementation ===

// safe value access
int64_t BenCodeVal::asInteger() const {
    return std::get<int64_t>(data);
}
const std::string &BenCodeVal::asString() const {
    return std::get<std::string>(data);
}
const std::vector<BenCodeVal> &BenCodeVal::asList() const {
    return std::get<std::vector<BenCodeVal>>(data);
}
const std::unordered_map<std::string, BenCodeVal> &BenCodeVal::asDict() const {
    return std::get<std::unordered_map<std::string, BenCodeVal>>(data);
}

// for modification
std::vector<BenCodeVal> &BenCodeVal::asList() {
    return std::get<std::vector<BenCodeVal>>(data);
}
std::unordered_map<std::string, BenCodeVal> &BenCodeVal::asDict() {
    return std::get<std::unordered_map<std::string, BenCodeVal>>(data);
}

std::string BenCodeVal::toString() const {
    switch (getType()) {
    case BenCodeType::INTEGER:
        return std::to_string(asInteger());
    case BenCodeType::STRING:
        return "\"" + asString() + "\"";
    case BenCodeType::LIST: {
        std::string result = "[";
        const auto &list = asList();
        for (size_t i = 0; i < list.size(); ++i) {
            if (i > 0)
                result += ", ";
            result += list[i].toString();
        }
        result += "]";
        return result;
    }
    case BenCodeType::DICT: {
        std::string result = "{";
        const auto &dict = asDict();
        bool first = true;
        for (const auto &pair : dict) {
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

// === BenCodeDecoder Implementation ===

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

// decode input
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
