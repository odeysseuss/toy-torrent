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
};

class BenCodeDecoder {
private:
    std::string content;
    std::size_t pos;

    char peek() const;
    void consume(char ch) const;

public:
    BenCodeDecoder(const std::string &input) : content(input), pos(0) {};

    std::shared_ptr<BValue> decode();
};

#endif
