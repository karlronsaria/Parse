#include "Lexer.h"

Lexer::Lexer(std::shared_ptr<IEnumerator> && stream):
    _stream(std::move(stream))
{
    NextChar();
}

bool Lexer::NextChar() {
    return _stream->Next(_current_char);
}

bool Lexer::IgnoreWhiteSpace() {
    // Starting position:
    //      v
    //   for   (int i = 0;
    // 
    // Ending position:
    //         v
    //   for   (int i = 0;
    // 
    while (isspace(_current_char) && NextChar()) {}
    return _stream->HasNext();
}

int Lexer::LexWhiteSpace() {
    // Starting position:
    //      v
    //   for   (int i = 0;
    // 
    // Ending position:
    //         v
    //   for   (int i = 0;
    // 
    _string = "";
    _string += _current_char;

    while (NextChar() && isspace(_current_char))
        _string += _current_char;

    return _stream->HasNext()
        ? Token::SPACE
        : Token::END;
}

int Lexer::LexWord() {
    // Starting position:
    //   v
    //   _what3
    // 
    // Ending position:
    //         v
    //   _what3
    // 
    _string = "";
    _string += _current_char;

    while (NextChar() && (isalpha(_current_char) || _current_char == '_' || isdigit(_current_char)))
        _string += _current_char;

    return _stream->HasNext()
        ? Token::WORD
        : Token::END;
}

int Lexer::LexInteger(int & digits) {
    // Starting position:
    //   v
    //   12.57
    // 
    // Ending position:
    //     v
    //   12.57
    // 
    _integer = (int)(_current_char - '0');
    digits = 1;

    while (NextChar() && isdigit(_current_char)) {
        _integer = (10 * _integer) + (int)(_current_char - '0');
        digits = digits + 1;
    }

    return _stream->HasNext()
        ? Token::INTEGER
        : Token::END;
}

int Lexer::LexInteger() {
    // Starting position:
    //   v
    //   12.57
    // 
    // Ending position:
    //     v
    //   12.57
    // 
    _integer = (int)(_current_char - '0');

    while (NextChar() && isdigit(_current_char)) {
        _integer = (10 * _integer) + (int)(_current_char - '0');
    }

    return _stream->HasNext()
        ? Token::INTEGER
        : Token::END;
}

int Lexer::LexString(char delimiter) {
    // Starting position:
    //    v
    //   "It's all I have to bring today"
    // 
    // Ending position:
    //                                   v
    //   "It's all I have to bring today"
    // 
    _string = "";
    _string += _current_char;

    while (NextChar() && _current_char != delimiter)
        _string += _current_char;

    NextChar();

    return _stream->HasNext()
        ? Token::STRING
        : Token::END;
}

int Lexer::LexString(
    char delimiter,
    int (*forEachCharacter)(Lexer &)
) {
    // Starting position:
    //    v
    //   "It's all I have to bring today"
    // 
    // Ending position:
    //                                   v
    //   "It's all I have to bring today"
    // 
    _string = "";
    _string += _current_char;
    int token = (*forEachCharacter)(*this);

    while (_stream->HasNext() && _current_char != delimiter) {
        switch ((Token)token) {
            case Token::END:
            case Token::ERROR:
                return Token::ERROR;
            default:
                _string += (char)token;
                break;
        }

        token = (*forEachCharacter)(*this);
    }

    NextChar();

    return _stream->HasNext()
        ? Token::STRING
        : Token::END;
}

int Lexer::LexCharacter(char delimiter) {
    // Starting position:
    //    v
    //   '\n'
    // 
    // Ending position:
    //       v
    //   '\n'
    // 
    if (!NextChar())
        return Token::ERROR;

    _character = _current_char;

    if (!NextChar() || _current_char != delimiter)
        return Token::ERROR;

    return _stream->HasNext()
        ? Token::CHARACTER
        : Token::END;
}

int Lexer::LexCharacter(
    char delimiter,
    int (*forEachCharacter)(Lexer &)
) {
    // Starting position:
    //    v
    //   '\n'
    // 
    // Ending position:
    //       v
    //   '\n'
    // 
    int token = (*forEachCharacter)(*this);

    switch (token) {
        case Token::END:
        case Token::ERROR:
            return Token::ERROR;
        default:
            _character += (char)token;
            break;
    }

    if (!NextChar() || _current_char != delimiter)
        return Token::ERROR;

    return _stream->HasNext()
        ? Token::CHARACTER
        : Token::END;
}

int Lexer::LexNumber() {
    // Starting position:
    //    v
    //   -12.57
    // 
    // Ending position:
    //         v
    //   -12.57
    // 
    int wholePart;
    int token = LexInteger();

    switch ((Token)token) {
        case Token::END:
            return Token::END;
        case Token::INTEGER:
            wholePart = Integer();
            break;
        default:
            // TODO: Error
            return Token::ERROR;
    }

    if (_current_char != '.')
        return Token::INTEGER;

    NextChar();

    int digits = 0;
    token = LexInteger(digits);

    if ((Token)token != Token::INTEGER)
        return Token::ERROR;

    int fractionPart = Integer();

    _float = fractionPart == 0
        ? wholePart
        : wholePart + (fractionPart / pow(10, digits));

    return Token::FLOAT;
}

std::shared_ptr<IEnumerator> Lexer::Enumerator() {
    return _stream;
}

int Lexer::NextToken() {
    if (!_stream->HasNext())
        return Token::END;

    if (isspace(_current_char))
        return LexWhiteSpace();

    if (isalpha(_current_char) || _current_char == '_')
        return LexWord();

    if (isdigit(_current_char))
        return LexInteger();

    int temp = (int)_current_char;
    NextChar();
    return temp;
}

int Lexer::Index() const {
    return _stream->Index();
}

bool Lexer::HasNext() const {
    return _stream->HasNext();
}

char Lexer::CurrentChar() const {
    return _current_char;
}

const std::string & Lexer::String() const {
    return _string;
}

int Lexer::Integer() const {
    return _integer;
}

float Lexer::Float() const {
    return _float;
}

char Lexer::Character() const {
    return _character;
}