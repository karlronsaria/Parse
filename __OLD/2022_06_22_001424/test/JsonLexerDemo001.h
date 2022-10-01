#pragma once
#ifndef _JSONLEXERDEMO001_H
#define _JSONLEXERDEMO001_H

#include "../src/Lexer.h"
#include "../src/JsonParser.h"

class JsonLexer: public IJsonLexer {
    private:
        Lexer _lexer;
    public:
        virtual ~IJsonLexer() = default;
        virtual int NextToken() override;
        virtual int Integer() const override;
        virtual float Float() const override;
        virtual const std::string & String() const override;

        // Exception-handling
        virtual char CurrentChar() const override;
        virtual int Index() const override;
};

int JsonLexer::NextToken() {
    return IJsonLexer::NextToken(_lexer);
}

int JsonLexer::Integer() {
    return _lexer.Integer();
}

float JsonLexer::Float() {
    return _lexer.Float();
}

const std::string & JsonLexer::String() {
    return _lexer.String();
}

char JsonLexer::CurrentChar() {
    return _lexer.CurrentChar();
}

int JsonLexer::Index() {
    return _lexer.Index();
}

#endif
