#ifndef BENCODE_HPP
#define BENCODE_HPP

#include <string>
#include <vector>
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

    // copy constructor
    BValue(const BValue &other) : type(other.type) {
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

    BValue &operator=(const BValue &other);

    void cleanup();
    ~BValue() {
        cleanup();
    }

    BType getType() const {
        return type;
    };

    int64_t asInt() const;
    const std::string asStr() const;
    const std::vector<BValue> asList() const;
    const std::unordered_map<std::string, BValue> asDict() const;

    std::string toString() const;
};

class BenCodeDecoder {
private:
    std::string content;
    std::size_t pos;

    char peek() const;
    char getChar();
    void consume(char ch);
    std::string readUntil(char delimeter);
    int64_t readInt();
    std::string readStr();
    std::vector<BValue> readList();
    std::unordered_map<std::string, BValue> readDict();
    BValue decodeValue();

public:
    BenCodeDecoder(const std::string &input) : content(input), pos(0) {};

    BValue decode();
};

#endif
