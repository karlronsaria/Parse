#pragma once
#ifndef _LEXERDEMO001_H
#define _LEXERDEMO001_H

#include "../lib/Lexer.h"

class ILexerVisitor {
    public:
        virtual void ForWhiteSpace(const std::string &) = 0;
        virtual void ForWord(const std::string &) = 0;
        virtual void ForInteger(int) = 0;
        virtual int ForPunctuation(char) = 0;
        virtual bool ForEachToken(int token) = 0;
        virtual std::shared_ptr<Lexer> GetLexer() = 0;
};

bool ForEachToken(Lexer &, std::shared_ptr<ILexerVisitor> &&);

#endif