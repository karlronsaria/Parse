#include "LexerDemo001.h"

bool ForEachToken(
    Lexer & lexer,
    std::shared_ptr<ILexerVisitor> && visitor
) {
    bool hasNext = true;
    int token = lexer.NextToken();

    while (hasNext) {
        switch ((Token)token) {
            case Token::SPACE:
                visitor->ForWhiteSpace(lexer.String());
                break;
            case Token::END:
                hasNext = false;
                break;
            case Token::WORD:
                visitor->ForWord(lexer.String());
                break;
            case Token::INTEGER:
                visitor->ForInteger(lexer.Integer());
                break;
            default:
                token = visitor->ForPunctuation((char)token);
                break;
        }

        if (hasNext) {
            if (!visitor->ForEachToken(token))
                return false;

            token = lexer.NextToken();
        }
    }

    return true;
}
