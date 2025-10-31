#include "bencode.hpp"
#include <stdexcept>

void BValue::cleanup() {
    switch (type) {
    case BType::STRING:
        BStr.~basic_string();
        break;
    case BType::LIST:
        BList.~vector();
        break;
    case BType::DICT:
        BDict.~unordered_map();
        break;
    default:
        break;
    };
}

BValue &BValue::operator=(const BValue &other) {
    if (this != &other) {
        cleanup();
        type = other.type;
        switch (type) {
        case BType::INTEGER:
            BInt = other.BInt;
            break;
        case BType::STRING:
            new (&BStr) std::string(other.BStr);
            break;
        case BType::LIST:
            new (&BList) std::vector<BValue>(other.BList);
            break;
        case BType::DICT:
            new (&BDict) std::unordered_map<std::string, BValue>(other.BDict);
            break;
        }
    }
    return *this;
}

int64_t BValue::asInt() const {
    if (type != BType::INTEGER)
        throw std::runtime_error("Not an Integer");
    return BInt;
}

const std::string BValue::asStr() const {
    if (type != BType::STRING)
        throw std::runtime_error("Not a String");
    return BStr;
}

const std::vector<BValue> BValue::asList() const {
    if (type != BType::LIST)
        throw std::runtime_error("Not a List");
    return BList;
}

const std::unordered_map<std::string, BValue> BValue::asDict() const {
    if (type != BType::DICT)
        throw std::runtime_error("Not a Dictionary");
    return BDict;
}

std::string BValue::toString() const {
    switch (type) {
    case BType::INTEGER:
        return std::to_string(BInt);
    case BType::STRING:
        return "\"" + BStr + "\"";
    case BType::LIST: {
        std::string result = "[";
        for (size_t i = 0; i < BList.size(); ++i) {
            if (i > 0)
                result += ", ";
            result += BList[i].toString();
        }
        result += "]";
        return result;
    }
    case BType::DICT: {
        std::string result = "{";
        bool first = true;
        for (const auto &pair : BDict) {
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

char BenCodeDecoder::peek() const {
    if (pos >= content.size())
        return '\0';
    return content[pos];
}

char BenCodeDecoder::getChar() {
    if (pos >= content.size())
        throw std::runtime_error("Unexpected end of input");
    return content[pos++];
}

void BenCodeDecoder::consume(char ch) {
    if (pos >= content.size() || content[pos] != ch)
        throw std::runtime_error("Unexpected input");
    pos++;
}

std::string BenCodeDecoder::readUntil(char delimeter) {
    std::size_t start = pos;
    while (pos < content.size() && content[pos] != delimeter) {
        pos++;
    }
    if (pos >= content.size())
        throw std::runtime_error("Unexpected end of input");

    return content.substr(start, pos - start);
}

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

    if (pos + length > content.size()) {
        throw std::runtime_error("Unexpected end of input");
    }

    std::string res = content.substr(pos, length);
    pos += length;
    return res;
}

std::vector<BValue> BenCodeDecoder::readList() {
    consume('l');
    std::vector<BValue> list;

    while (peek() != 'e') {
        list.push_back(decodeValue());
    }

    consume('e');
    return list;
}

std::unordered_map<std::string, BValue> BenCodeDecoder::readDict() {
    consume('d');
    std::unordered_map<std::string, BValue> dict;

    while (peek() != 'e') {
        std::string key = readStr();
        BValue val = decodeValue();
        dict[key] = val;
    }

    consume('e');
    return dict;
}

BValue BenCodeDecoder::decodeValue() {
    char ch = peek();

    switch (ch) {
    case 'i':
        return BValue(readInt());
    case 'l':
        return BValue(readList());
    case 'd':
        return BValue(readDict());
    default:
        if (isdigit(ch)) {
            return BValue(readStr());
        } else {
            throw std::runtime_error("Unexpected char in bencode String");
        }
    }
}

BValue BenCodeDecoder::decode() {
    BValue res = decodeValue();
    if (pos != content.size()) {
        throw std::runtime_error("Extra data after bencoded value");
    }

    return res;
}
