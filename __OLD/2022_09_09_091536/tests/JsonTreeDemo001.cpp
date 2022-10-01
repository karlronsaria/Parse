/*
#include "JsonTreeDemo001.h"

std::ostream & MyJsonTreeVisitor::PutIndent() {
    if (_level)
        _stream << std::setw(INCREMENT * _level) << ' '; 

    return _stream;
}

int MyJsonTreeVisitor::Level() const {
    return _level;
}

void MyJsonTreeVisitor::Push() {
    PutIndent();
    _stream << "(\n";
    _level++;
}

void MyJsonTreeVisitor::Pop() {
    _level--;
    PutIndent();
    _stream << ")\n";
}

void MyJsonTreeVisitor::StartObject() {
    PutIndent();
    _stream << "New Object\n";
    Push();
}

void MyJsonTreeVisitor::EndObject() {
    Pop();
}

void MyJsonTreeVisitor::StartList() {
    PutIndent();
    _stream << "New List\n";
    Push();
}

void MyJsonTreeVisitor::EndList() {
    Pop();
}

void MyJsonTreeVisitor::StartPair(const std::string & key) {
    PutIndent();
    _stream << "New Pair\n";
    Push();

    PutIndent();
    _stream << "Key: [" << key << "]\n";
}

void MyJsonTreeVisitor::EndPair() {
    Pop();
}

void MyJsonTreeVisitor::StartValue() {
    PutIndent();
    _stream << "Value:\n";
    Push();
}

void MyJsonTreeVisitor::EndValue() {
    Pop();
}

void MyJsonTreeVisitor::ForNumeric(Homonumeric value) {
    PutIndent();

    switch (value.Mode) {
        case Homonumeric::Mode::BOOLEAN:
            _stream << "Boolean: [" << value.Boolean << "]\n";
            break;
        case Homonumeric::Mode::INTEGER:
            _stream << "Integer: [" << value.Integer << "]\n";
            break;
        case Homonumeric::Mode::FLOAT:
            _stream << "Float: [" << value.Float << "]\n";
            break;
        default:
            break;
    }
}

void MyJsonTreeVisitor::ForString(const std::string & value) {
    PutIndent();
    _stream << "String: [" << value << "]\n";
}
*/