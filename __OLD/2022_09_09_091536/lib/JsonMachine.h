#pragma once
#ifndef _JSONMACHINE_H
#define _JSONMACHINE_H

#include "VectorBitset.h"
#include <memory>
#include <sstream>
#include <unordered_map>

// **note: sfinae
//   link: https://en.wikipedia.org/wiki/Substitution_failure_is_not_an_error#:~:text=Substitution%20failure%20is%20not%20an%20error%20(SFINAE)%20refers%20to%20a,to%20describe%20related%20programming%20techniques.
//   retrieved: 2022_07_26

namespace Json {
    enum Type {
        STRING,
        INTEGER,
        FLOAT,
        BOOLEAN,
        OBJECT,
        LIST,
        NIL
    };

    std::string
    ToString(Type);

    typedef int
    key_t;

    struct Pointer {
        Type type = Type::NIL;
        key_t key = 0;
    };

    typedef std::vector<Pointer>
    list_t;

    template <typename T>
    class ObjectDefinition {
        private:
            std::unordered_map<T, Pointer>
            _map;

            std::vector<T>
            _keys;
        public:
            virtual ~ObjectDefinition() = default;

            size_t
            size() const {
                return _map.size();
            }

            bool
            hasKey(const T & key) const {
                return _map.find(key) != _map.end();
            }

            bool
            add(const T & key, Pointer value) {
                if (hasKey(key))
                    return false;

                _map[key] = value;
                _keys.push_back(key);
                return true;
            }

            Pointer
            at(const T & key) const {
                if (hasKey(key))
                    return _map.at(key);

                return Pointer();
            }

            Pointer
            operator[](const T & key) const {
                return at(key);
            }

            Pointer &
            operator[](const T & key) {
                return _map[key];
            }

            const std::vector<std::string> &
            keys() const {
                return _keys;
            }
    };

    typedef ObjectDefinition<std::string>
    object_t;

    class Machine {
        private:
            typedef
            Machine const *
            machine_ptr_t;
        public:
            template <typename Machine_Pointer_Type = machine_ptr_t>
            class ResultSet {
                friend class Machine;

                private:
                    Machine_Pointer_Type
                    _machine;

                    Pointer
                    _pointer;

                    std::string
                    RecurseToString(Pointer) const;

                    ResultSet(Machine_Pointer_Type, const Pointer &);
                public:
                    ResultSet();
                    ResultSet(const ResultSet &) = default;
                    virtual ~ResultSet() = default;
                    ResultSet & operator=(const ResultSet &) = default;
                    bool AsInteger(int &) const;
                    bool AsFloat(float &) const;
                    bool AsString(std::string &) const;
                    ResultSet At(int) const;
                    ResultSet At(const std::string &) const;
                    ResultSet operator[](int) const;
                    ResultSet operator[](const std::string &) const;
                    std::vector<ResultSet> Where(bool (*filter)(ResultSet)) const;
                    int Compare(ResultSet) const;
                    bool Equals(const std::string &) const;
                    Json::Type TypeCode() const;
                    bool IsNil() const;
                    std::string ToString() const;

                    bool ChangeString(const std::string &);
                    bool ChangeInteger(int);
                    bool ChangeFloat(float);
            };
        private:
            std::vector<std::string>
            _strings;

            std::vector<int>
            _integers;

            std::vector<float>
            _floats;

            vector_bitset<>
            _booleans;

            std::vector<list_t>
            _lists;

            std::vector<object_t>
            _objects;

            key_t
            _start;
        public:
            virtual ~Machine() = default;
            Machine();

            void SetStartingObject(key_t);

            Pointer NewObject();
            Pointer NewList();
            Pointer NewString(const std::string &);
            Pointer NewInteger(int);
            Pointer NewFloat(float);
            Pointer NewBoolean(bool);

            object_t & Object(key_t);
            list_t & List(key_t);
            std::string & String(key_t);
            int & Integer(key_t);
            float & Float(key_t);
            void SetBoolean(key_t, bool);

            const object_t & Object(key_t) const;
            const list_t & List(key_t) const;
            const std::string & String(key_t) const;
            const int & Integer(key_t) const;
            const float & Float(key_t) const;
            bool Boolean(key_t) const;

            std::string
            ToString() const;

            const ResultSet<const Machine *>
            GetResultSet() const;

            ResultSet<Machine *>
            GetResultSet();

            const ResultSet<const Machine *>
            GetResultSet(key_t start) const;

            ResultSet<Machine *>
            GetResultSet(key_t start);
    };
};

template <typename T>
Json::Machine::ResultSet<T>::ResultSet(
    T machine,
    const Json::Pointer & pointer
):  _machine(machine),
    _pointer(pointer) {}

template <typename T>
Json::Machine::ResultSet<T>::ResultSet():
    _machine(nullptr),
    _pointer(Json::Pointer{ Json::Type::NIL, 0 }) {}

#define DEFINE_JSONMACHINE_RESULTSET_GETTER(NAME, TYPE, TYPE_SYMBOL) \
    template <typename T> \
    bool \
    Json::Machine::ResultSet<T>::As##NAME(TYPE & value) const { \
        if (_pointer.type != TYPE_SYMBOL) \
            return false; \
        /* TODO: sfinae **see above */ \
        value = _machine->NAME(_pointer.key); \
        return true; \
    }

DEFINE_JSONMACHINE_RESULTSET_GETTER(Integer, int, Type::INTEGER)
DEFINE_JSONMACHINE_RESULTSET_GETTER(Float, float, Type::FLOAT)
DEFINE_JSONMACHINE_RESULTSET_GETTER(String, std::string, Type::STRING)
#undef DEFINE_JSONMACHINE_RESULTSET_GETTER

template <typename T>
typename Json::Machine::ResultSet<T>
Json::Machine::ResultSet<T>::At(int index) const {
    if (_pointer.type != Type::LIST)
        return ResultSet(_machine, Pointer{ Type::NIL, 0 });

    /* TODO: sfinae **see above */
    return ResultSet(_machine, _machine->List(_pointer.key).at(index));
}

template <typename T>
typename Json::Machine::ResultSet<T>
Json::Machine::ResultSet<T>::At(const std::string & key) const {
    if (_pointer.type != Type::OBJECT)
        return ResultSet(_machine, Pointer{ Type::NIL, 0 });

    /* TODO: sfinae **see above */
    return ResultSet(_machine, _machine->Object(_pointer.key).at(key));
}

template <typename T>
typename Json::Machine::ResultSet<T>
Json::Machine::ResultSet<T>::operator[](int index) const {
    return At(index);
}

template <typename T>
typename Json::Machine::ResultSet<T>
Json::Machine::ResultSet<T>::operator[](const std::string & key) const {
    return At(key);
}

template <typename T>
std::vector<Json::Machine::ResultSet<T>>
Json::Machine::ResultSet<T>::Where(bool (*filter)(ResultSet)) const {
    std::vector<ResultSet> results;

    if (_pointer.type != Type::LIST) {
        results.push_back(
            ResultSet(
                _machine,
                Pointer{ Type::NIL, 0 }
            )
        );

        return std::move(results);
    }

    /* TODO: sfinae **see above */
    auto list = _machine->List(_pointer.key);

    for (Pointer pointer : list) {
        ResultSet temp(_machine, pointer);

        if (filter(temp))
            results.push_back(temp);
    }

    return std::move(results);
}

template <typename T>
int
Json::Machine::ResultSet<T>::Compare(ResultSet other) const {
    return ToString().compare(other.ToString());
}

template <typename T>
bool
Json::Machine::ResultSet<T>::Equals(const std::string & toString) const {
    return !ToString().compare(toString);
}

template <typename T>
std::string
Json::Machine::ResultSet<T>::RecurseToString(
    Json::Pointer value
) const {
    std::ostringstream outss;

    switch (value.type) {
        case Type::STRING:
            outss
                << '"'
                /* TODO: sfinae **see above */
                << _machine->String(value.key)
                << '"';

            break;
        default:
            outss
                << ResultSet(_machine, value).ToString();

            break;
    }

    return outss.str();
}

template <typename T>
Json::Type
Json::Machine::ResultSet<T>::TypeCode() const {
    return _pointer.type;
}

template <typename T>
bool
Json::Machine::ResultSet<T>::IsNil() const {
    return _pointer.type == Type::NIL;
}

template <typename T>
std::string
Json::Machine::ResultSet<T>::ToString() const {
    std::ostringstream outss;

    switch (_pointer.type) {
        case Json::Type::STRING:
            /* TODO: sfinae **see above */
            return _machine->String(_pointer.key);
        case Json::Type::INTEGER:
            /* TODO: sfinae **see above */
            outss << _machine->Integer(_pointer.key);
            break;
        case Json::Type::FLOAT:
            /* TODO: sfinae **see above */
            outss << _machine->Float(_pointer.key);
            break;
        case Json::Type::BOOLEAN:
            outss << (
                /* TODO: sfinae **see above */
                _machine->Boolean(_pointer.key)
                    ? "true" : "false"
            );
            break;
        case Json::Type::OBJECT:
            {
                outss << "{ ";
                /* TODO: sfinae **see above */
                auto object = _machine->Object(_pointer.key);
                std::string key;

                for (int i = 0; i < object.keys().size(); ++i) {
                    key = object.keys()[i];

                    outss
                        << '"'
                        << key
                        << "\": "
                        << RecurseToString(object.at(key));

                    if (i < object.keys().size() - 1)
                        outss << ", ";
                }

                outss << " }";
            }

            break;
        case Json::Type::LIST:
            {
                outss << "[ ";
                /* TODO: sfinae **see above */
                auto list = _machine->List(_pointer.key);

                for (int i = 0; i < list.size(); ++i) {
                    outss << RecurseToString(list.at(i));

                    if (i < list.size() - 1)
                        outss << ", ";
                }

                outss << " ]";
            }

            break;
        case Json::Type::NIL:
            return "";
    }

    return outss.str();
}

template <typename T>
bool Json::Machine::ResultSet<T>::ChangeString(const std::string & value) {
    if (TypeCode() != Type::STRING)
        return false;

    /* TODO: sfinae **see above */
    _machine->String(_pointer.key) = value;
    return true;
}

template <typename T>
bool Json::Machine::ResultSet<T>::ChangeInteger(int value) {
    switch (TypeCode()) {
        case Type::INTEGER:
            /* TODO: sfinae **see above */
            _machine->Integer(_pointer.key) = value;
            return true;
        case Type::BOOLEAN:
            /* TODO: sfinae **see above */
            _machine->SetBoolean(_pointer.key, value);
            return true;
        default:
            break;
    }

    return false;
}

template <typename T>
bool Json::Machine::ResultSet<T>::ChangeFloat(float value) {
    if (TypeCode() != Type::FLOAT)
        return false;

    /* TODO: sfinae **see above */
    _machine->Float(_pointer.key) = value;
    return true;
}

#endif
