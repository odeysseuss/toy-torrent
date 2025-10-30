#include "bencode.hpp"
#include <stdexcept>

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
