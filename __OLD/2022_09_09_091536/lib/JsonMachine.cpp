#include "JsonMachine.h"

std::string
Json::ToString(Json::Type typeCode) {
    switch (typeCode) {
        case Json::Type::STRING:
            return "String";
        case Json::Type::INTEGER:
            return "Integer";
        case Json::Type::FLOAT:
            return "Float";
        case Json::Type::BOOLEAN:
            return "Boolean";
        case Json::Type::OBJECT:
            return "Object";
        case Json::Type::LIST:
            return "List";
        case Json::Type::NIL:
            return "Nil";
    }

    return "";
}

Json::Machine::Machine():
    _start(-1) {}

void
Json::Machine::SetStartingObject(key_t start) {
    _start = start;
}

Json::Pointer
Json::Machine::NewObject() {
    _objects.push_back(std::move(Json::object_t()));
    return Json::Pointer {
        Json::Type::OBJECT,
        (int)_objects.size() - 1
    };
}

Json::Pointer
Json::Machine::NewList() {
    _lists.push_back(std::move(Json::list_t()));
    return Json::Pointer {
        Json::Type::LIST,
        (int)_lists.size() - 1
    };
}

#define DEFINE_JSONMACHINE_BUILDER(NAME, PARAM_TYPE, VECTOR, TYPE_SYMBOL) \
    Json::Pointer \
    Json::Machine::New##NAME(PARAM_TYPE value) { \
        VECTOR.push_back(value); \
        return Json::Pointer { \
            TYPE_SYMBOL, \
            (int)VECTOR.size() - 1 \
        }; \
    }

DEFINE_JSONMACHINE_BUILDER(String, const std::string &, _strings, Json::Type::STRING)
DEFINE_JSONMACHINE_BUILDER(Integer, int, _integers, Json::Type::INTEGER)
DEFINE_JSONMACHINE_BUILDER(Float, float, _floats, Json::Type::FLOAT)
DEFINE_JSONMACHINE_BUILDER(Boolean, bool, _booleans, Json::Type::BOOLEAN)
#undef DEFINE_JSONMACHINE_BUILDER

#define DEFINE_JSONMACHINE_GETTER(RETURN, NAME, RESOURCE) \
    RETURN & \
    Json::Machine::NAME(key_t key) { \
        return RESOURCE[key]; \
    }

DEFINE_JSONMACHINE_GETTER(Json::object_t, Object, _objects)
DEFINE_JSONMACHINE_GETTER(Json::list_t, List, _lists)
DEFINE_JSONMACHINE_GETTER(std::string, String, _strings)
DEFINE_JSONMACHINE_GETTER(int, Integer, _integers)
DEFINE_JSONMACHINE_GETTER(float, Float, _floats)
#undef DEFINE_JSONMACHINE_GETTER

void
Json::Machine::SetBoolean(key_t key, bool value) {
    _booleans.set((size_t)key, value);
}

#define DEFINE_JSONMACHINE_CONST_GETTER(RETURN, NAME, RESOURCE) \
    const RETURN & \
    Json::Machine::NAME(key_t key) const { \
        return RESOURCE.at(key); \
    }

DEFINE_JSONMACHINE_CONST_GETTER(Json::object_t, Object, _objects)
DEFINE_JSONMACHINE_CONST_GETTER(Json::list_t, List, _lists)
DEFINE_JSONMACHINE_CONST_GETTER(std::string, String, _strings)
DEFINE_JSONMACHINE_CONST_GETTER(int, Integer, _integers)
DEFINE_JSONMACHINE_CONST_GETTER(float, Float, _floats)
#undef DEFINE_JSONMACHINE_GETTER

bool
Json::Machine::Boolean(key_t key) const {
    return _booleans[(size_t)key];
}

std::string
Json::Machine::ToString() const {
    std::ostringstream oss;

    oss << "Objects:\n";

    for (int i = 0; i < _objects.size(); ++i) {
        oss << "  " << i << ":\n";
        auto & object = _objects[i];

        for (int j = 0; j < object.keys().size(); ++j) {
            auto & key = object.keys().at(j);
            auto value = object.at(key);

            oss << "    Pair " << j << ":\n"
                << "      Key: [" << key << "]\n"
                << "      Value:\n"
                << "        Type: " << Json::ToString(value.type) << '\n'
                << "        Index: " << value.key << '\n';
        }
    }

    oss << "Lists:\n";

    for (int i = 0; i < _lists.size(); ++i) {
        oss << "  " << i << ":\n";
        auto & list = _lists[i];

        for (auto & value : list) {
            oss << "    Type: " << Json::ToString(value.type) << '\n'
                << "    Index: " << value.key << '\n';
        }
    }

    oss << "Strings:\n";

    for (int i = 0; i < _strings.size(); ++i)
        oss << "  " << i << ": [" << _strings[i] << "]\n";

    oss << "Integers:\n";

    for (int i = 0; i < _integers.size(); ++i)
        oss << "  " << i << ": [" << _integers[i] << "]\n";

    oss << "Floats:\n";

    for (int i = 0; i < _floats.size(); ++i)
        oss << "  " << i << ": [" << _floats[i] << "]\n";

    oss << "Booleans:\n";

    for (int i = 0; i < _booleans.size(); ++i)
        oss << "  " << i << ": ["
            << (_booleans[i] ? "true" : "false")
            << "]\n";

    return oss.str();
}

#define BLOCK_JSONMACHINE_GETRESULTSET(PARAM_TYPE, START) \
    ResultSet<PARAM_TYPE>( \
        this, \
        _objects.empty() \
            ? Pointer{ Type::NIL, 0 } \
            : Pointer{ \
                Type::OBJECT, \
                START < 0 \
                    ? (int)_objects.size() - 1 \
                    : START \
            } \
    )

typename Json::Machine::ResultSet<Json::Machine *>
Json::Machine::GetResultSet() {
    return BLOCK_JSONMACHINE_GETRESULTSET(Json::Machine *, _start);
}

const typename Json::Machine::ResultSet<Json::Machine const *>
Json::Machine::GetResultSet() const {
    return BLOCK_JSONMACHINE_GETRESULTSET(Json::Machine const *, _start);
}

typename Json::Machine::ResultSet<Json::Machine *>
Json::Machine::GetResultSet(key_t start) {
    return BLOCK_JSONMACHINE_GETRESULTSET(Json::Machine *, start);
}

const typename Json::Machine::ResultSet<Json::Machine const *>
Json::Machine::GetResultSet(key_t start) const {
    return BLOCK_JSONMACHINE_GETRESULTSET(Json::Machine const *, start);
}

#undef BLOCK_JSONMACHINE_GETRESULTSET
