#include "JsonParser.h"

int Json::Parsing::Escape(::Lexer & lexer) {
    if (!lexer.NextChar())
        return Token::ERROR;

    char character = lexer.CurrentChar();

    if (character != '\\')
        return character;

    if (!lexer.NextChar())
        return Token::ERROR;

    switch (lexer.CurrentChar()) {
        case 'a': return '\a';
        case 'b': return '\b';
        case 'e': return '\e';
        case 'n': return '\n';
        case 'r': return '\r';
        case 't': return '\t';
        case 'v': return '\v';
        case '\\': return '\\';
        case '\'': return '\'';
        case '"': return '\"';
        case '?': return '\?';
        default: return Token::ERROR;
    }
}

int Json::Parsing::NextToken(::Lexer & lexer) {
    if (!lexer.IgnoreWhiteSpace())
        return Token::END;

    char currentChar = lexer.CurrentChar();

    if (isalpha(currentChar) || currentChar == '_')
        return lexer.LexWord();

    if (isdigit(currentChar))
        return lexer.LexNumber();

    if (currentChar == '"') {
        lexer.NextChar();
        return lexer.LexString('"', Escape);
    }

    if (currentChar == '\'') {
        lexer.NextChar();
        return lexer.LexCharacter('\'', Escape);
    }

    int temp = (int)currentChar;
    lexer.NextChar();
    return temp;
}

Json::Lexer::Lexer(
    std::shared_ptr<IEnumerator> && stream
):  _lexer(std::move(stream)) {}

int
Json::Lexer::NextToken() {
    return Json::Parsing::NextToken(_lexer);
}

const std::string &
Json::Lexer::String() const {
    return _lexer.String();
}

int
Json::Lexer::Integer() const {
    return _lexer.Integer();
}

float
Json::Lexer::Float() const {
    return _lexer.Float();
}

int
Json::Lexer::Index() const {
    return _lexer.Index();
}
