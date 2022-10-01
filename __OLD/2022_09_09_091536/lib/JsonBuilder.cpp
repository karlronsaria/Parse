#include "JsonBuilder.h"

void 
Json::Builder::PushStack(Symbol symbol, Pointer pointer) {
    _symbols.push(symbol);
    _pointers.push(pointer);
}

void 
Json::Builder::PopStack() {
    _symbols.pop();
    _pointers.pop();
}

void
Json::Builder::PushBack(Json::Pointer value) {
    auto & list = _machine->List(_pointers.top().key);
    list.push_back(value);
}

void
Json::Builder::SetValue(Json::Pointer value) {
    auto & object = _machine->Object(_pointers.top().key);
    object.add(_keys.top(), value);
    _keys.pop();
}

Json::Builder::Builder():
    _machine(std::make_shared<Json::Machine>())
{
    _machine->SetStartingObject(0);
}

Json::Builder::Builder(
    std::shared_ptr<Json::Machine> machine
):  _machine(machine) {}

bool
Json::Builder::AddValueToMachine(Json::Pointer pointer) {
    switch (_symbols.top()) {
        case Symbol::PAIR:
            SetValue(pointer);
            break;
        case Symbol::VALUE:
            PushBack(pointer);
            break;
        default:
            return false;
    }

    PopStack();
    return true;
}

Json::Builder &
Json::Builder::Object() {
    Pointer pointer = _machine->NewObject();

    if (_symbols.empty()) {
        _pointers.push(pointer);
        _symbols.push(Symbol::OBJECT);
        return *this;
    }

    if (!AddValueToMachine(pointer))
        return *this;

    PushStack(Symbol::OBJECT, pointer);
    return *this;
}

Json::Builder &
Json::Builder::List() {
    Pointer pointer = _machine->NewList();

    if (_symbols.empty() || !AddValueToMachine(pointer))
        return *this;

    PushStack(Symbol::LIST, pointer);
    return *this;
}

Json::Builder &
Json::Builder::Pair(const std::string & key) {
    if (_symbols.empty())
        return *this;

    switch (_symbols.top()) {
        case Symbol::LIST:
            PopStack();
            return Pair(key);
        case Symbol::OBJECT:
            _keys.push(key);
            PushStack(Symbol::PAIR, _pointers.top());
            break;
        default:
            break;
    }

    return *this;
}

Json::Builder &
Json::Builder::Value() {
    if (_symbols.empty())
        return *this;

    switch (_symbols.top()) {
        case Symbol::OBJECT:
            PopStack();
            return Value();
        case Symbol::LIST:
            PushStack(Symbol::VALUE, _pointers.top());
            break;
        default:
            break;
    }

    return *this;
}

Json::Builder &
Json::Builder::Pop() {
    if (!_symbols.empty())
        PopStack();

    return *this;
}

Json::Builder &
Json::Builder::String(const std::string & value) {
    _symbols.empty()
        || !AddValueToMachine(_machine->NewString(value));
    return *this;
}

Json::Builder &
Json::Builder::Integer(int value) {
    _symbols.empty()
        || !AddValueToMachine(_machine->NewInteger(value));
    return *this;
}

Json::Builder &
Json::Builder::Float(float value) {
    _symbols.empty()
        || !AddValueToMachine(_machine->NewFloat(value));
    return *this;
}

Json::Builder &
Json::Builder::Boolean(bool value) {
    _symbols.empty()
        || !AddValueToMachine(_machine->NewBoolean(value));
    return *this;
}

std::shared_ptr<const Json::Machine>
Json::Builder::Machine() const {
    return _machine;
}
