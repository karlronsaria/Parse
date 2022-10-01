#include "LexerDemo002.h"

int LexerDemo002::EscapeC(Lexer & lexer) {
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

int LexerDemo002::NextTokenC(Lexer & lexer) {
    if (!lexer.IgnoreWhiteSpace())
        return Token::END;

    char currentChar = lexer.CurrentChar();

    if (isalpha(currentChar) || currentChar == '_')
        return lexer.LexWord();

    if (isdigit(currentChar))
        return lexer.LexNumber();

    if (currentChar == '"') {
        lexer.NextChar();
        return lexer.LexString('"', EscapeC);
    }

    if (currentChar == '\'') {
        lexer.NextChar();
        return lexer.LexCharacter('\'', EscapeC);
    }

    int temp = (int)currentChar;
    lexer.NextChar();
    return temp;
}