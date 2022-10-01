#include "JsonParserDemo001.h"

std::ostream & MyJsonParserVisitor::PutIndent() {
    if (_level)
        _stream << std::setw(INCREMENT * _level) << ' '; 

    return _stream;
}

int MyJsonParserVisitor::Level() const {
    return _level;
}

void MyJsonParserVisitor::Push() {
    PutIndent();
    _stream << "(\n";
    _level++;
}

void MyJsonParserVisitor::Pop() {
    _level--;
    PutIndent();
    _stream << ")\n";
}

bool MyJsonParserVisitor::ForJson(Lexer & lexer) {
    PutIndent();
    _stream << "Start Json\n";
    return true;
}

bool MyJsonParserVisitor::ForObject(Lexer & lexer) {
    PutIndent();
    _stream << "New Object\n";
    return true;
}

bool MyJsonParserVisitor::ForList(Lexer & lexer) {
    PutIndent();
    _stream << "New List\n";
    return true;
}

bool MyJsonParserVisitor::ForPair(Lexer & lexer) {
    PutIndent();
    _stream << "New Pair\n";
    return true;
}

bool MyJsonParserVisitor::ForValue(Lexer & lexer) {
    PutIndent();
    _stream << "Value:\n";
    return true;
}

bool MyJsonParserVisitor::ForPrimitive(Lexer & lexer) {
    // Do nothing
    return true;
}

bool MyJsonParserVisitor::ForNumber(Lexer & lexer) {
    // Do nothing
    return true;
}

bool MyJsonParserVisitor::ForKeyWord(Lexer & lexer) {
    const std::string & str = lexer.String();

    if (!str.compare("true")) {
        PutIndent();
        _stream << "Boolean: [true]\n";
        return true;
    }

    if (!str.compare("false")) {
        PutIndent();
        _stream << "Boolean: [false]\n";
        return true;
    }

    if (!str.compare("null")) {
        PutIndent();
        _stream << "nil\n";
        return true;
    }

    return false;
}

bool MyJsonParserVisitor::ForInteger(Lexer & lexer) {
    PutIndent();
    _stream << "Integer: [" << lexer.Integer() << "]\n";
    return true;
}

bool MyJsonParserVisitor::ForFloat(Lexer & lexer) {
    PutIndent();
    _stream << "Float: [" << lexer.Float() << "]\n";
    return true;
}

bool MyJsonParserVisitor::ForString(Lexer & lexer) {
    PutIndent();
    _stream << "String: [" << lexer.String() << "]\n";
    return true;
}

bool MyJsonParserVisitor::ForError(Lexer & lexer) {
    _stream
        << "\n[!] Error at index "
        << lexer.Index()
        << " (character '"
        << lexer.CurrentChar()
        << "')\n";
    return true;
}

bool MyJsonParserVisitor::ForKey(Lexer & lexer) {
    PutIndent();
    _stream << "Key: [" << lexer.String() << "]\n";
    return true;
}
