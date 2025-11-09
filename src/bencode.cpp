#include "bencode.hpp"
#include <algorithm>
#include <stdexcept>

// === BVal Implementation ===

// safe value access
int64_t BVal::asInteger() const {
    return std::get<int64_t>(data);
}
const std::string &BVal::asString() const {
    return std::get<std::string>(data);
}
const std::vector<BVal> &BVal::asList() const {
    return std::get<std::vector<BVal>>(data);
}
const std::unordered_map<std::string, BVal> &BVal::asDict() const {
    return std::get<std::unordered_map<std::string, BVal>>(data);
}

// for modification
std::vector<BVal> &BVal::asList() {
    return std::get<std::vector<BVal>>(data);
}
std::unordered_map<std::string, BVal> &BVal::asDict() {
    return std::get<std::unordered_map<std::string, BVal>>(data);
}

std::string BVal::toString() const {
    switch (getType()) {
    case BType::INTEGER:
        return std::to_string(asInteger());
    case BType::STRING:
        return "\"" + asString() + "\"";
    case BType::LIST: {
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
    case BType::DICT: {
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

// === BDecode Implementation ===

// helper functions
char BDecode::peek() const {
    if (pos >= input.size())
        return '\0';
    return input[pos];
}

char BDecode::getChar() {
    if (pos >= input.size())
        throw std::runtime_error("Unexpected end of input");
    return input[pos++];
}

void BDecode::consume(char ch) {
    if (pos >= input.size() || input[pos] != ch)
        throw std::runtime_error("Unexpected input");
    pos++;
}

std::string BDecode::readUntil(char delimeter) {
    std::size_t start = pos;
    while (pos < input.size() && input[pos] != delimeter) {
        pos++;
    }
    if (pos >= input.size())
        throw std::runtime_error("Unexpected end of input");

    return input.substr(start, pos - start);
}

// read input
int64_t BDecode::readInt() {
    consume('i');
    std::string numStr = readUntil('e');
    consume('e');

    try {
        return std::stoll(numStr);
    } catch (const std::exception &e) {
        throw std::runtime_error("Invalid Integer format");
    }
}

std::string BDecode::readStr() {
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

std::vector<BVal> BDecode::readList() {
    consume('l');
    std::vector<BVal> list;

    while (peek() != 'e') {
        list.push_back(decodeValue());
    }

    consume('e');
    return list;
}

std::unordered_map<std::string, BVal> BDecode::readDict() {
    consume('d');
    std::unordered_map<std::string, BVal> dict;

    while (peek() != 'e') {
        std::string key = readStr();
        BVal val = decodeValue();
        dict.emplace(std::move(key), std::move(val));
    }

    consume('e');
    return dict;
}

// decode input
BVal BDecode::decodeValue() {
    char ch = peek();

    switch (ch) {
    case 'i':
        return BVal(readInt());
    case 'l':
        return BVal(readList());
    case 'd':
        return BVal(readDict());
    default:
        if (isdigit(ch)) {
            return BVal(readStr());
        } else {
            throw std::runtime_error("Unexpected char in bencode String");
        }
    }
}

// core decoder method
BVal BDecode::decode() {
    BVal res = decodeValue();
    if (pos != input.size()) {
        throw std::runtime_error("Extra data after bencoded value");
    }

    return res;
}

// === BEncoder Implementation ===

std::string BEncode::encodeInteger(int64_t value) {
    return "i" + std::to_string(value) + "e";
}

std::string BEncode::encodeString(const std::string &value) {
    return std::to_string(value.length()) + ":" + value;
}

std::string BEncode::encodeList(const std::vector<BVal> &list) {
    std::string result = "l";
    for (const auto &item : list) {
        result += encode(item);
    }
    result += "e";
    return result;
}

std::string BEncode::encodeDict(const std::unordered_map<std::string, BVal> &dict) {
    std::string result = "d";

    // Bencode requires dictionary keys to be in lexicographical order
    std::vector<std::string> keys;
    keys.reserve(dict.size());
    for (const auto &pair : dict) {
        keys.push_back(pair.first);
    }
    std::sort(keys.begin(), keys.end());

    for (const auto &key : keys) {
        result += encodeString(key);
        result += encode(dict.at(key));
    }

    result += "e";
    return result;
}

std::string BEncode::encode(const BVal &value) {
    switch (value.getType()) {
    case BType::INTEGER:
        return encodeInteger(value.asInteger());
    case BType::STRING:
        return encodeString(value.asString());
    case BType::LIST:
        return encodeList(value.asList());
    case BType::DICT:
        return encodeDict(value.asDict());
    default:
        throw std::runtime_error("Unknown BType for encoding");
    }
}
