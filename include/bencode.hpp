#ifndef BENCODE_HPP
#define BENCODE_HPP

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

enum class BType {
    INTEGER,
    STRING,
    LIST,
    DICT,
};

class BValue {
private:
    BType type;
    union {
        int64_t BInt;
        std::string BStr;
        std::vector<BValue> BList;
        std::unordered_map<std::string, BValue> BDict;
    };

public:
    BValue() : type(BType::INTEGER), BInt(0) {};
    BValue(const int64_t &val) : type(BType::INTEGER), BInt(val) {};
    BValue(const std::string &val) : type(BType::STRING), BStr(val) {};
    BValue(const std::vector<BValue> &val) : type(BType::LIST), BList(val) {};
    BValue(const std::unordered_map<std::string, BValue> &val) : type(BType::DICT), BDict(val) {};

    ~BValue() {
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

    BType getType() const {
        return type;
    };

    int64_t asInt() const;
    const std::string asStr() const;
    const std::vector<BValue> asList() const;
    const std::unordered_map<std::string, BValue> asDict() const;
};

class BenCodeDecoder {
private:
    std::string content;
    std::size_t pos;

    char peek() const;
    char getChar() const;
    void consume(char ch);
    std::string readUntil(char delimeter);
    int64_t readInt() const;
    std::string readStr() const;
    std::vector<BValue> readList() const;
    std::unordered_map<std::string, BValue> readDict() const;
    BValue decodeValue() const;

public:
    BenCodeDecoder(const std::string &input) : content(input), pos(0) {};

    std::shared_ptr<BValue> decode();
};

#endif
