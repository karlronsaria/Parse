#pragma once
#ifndef _JSONPARSERDEMO001_H
#define _JSONPARSERDEMO001_H

#include "../src/JsonParser.h"
#include <iostream>
#include <iomanip>

class MyJsonParserVisitor: public IJsonParserVisitor {
    public:
        static const int STARTING_LEVEL = 0;
        static const int INCREMENT = 2;
    private:
        std::ostream & _stream;
        int _level;
    public:
        std::ostream & PutIndent();
        int Level() const;

        MyJsonParserVisitor(std::ostream & stream):
            _stream(stream),
            _level(STARTING_LEVEL) {}
        virtual void Push();
        virtual void Pop();
        virtual bool ForJson(Lexer &) override;
        virtual bool ForObject(Lexer &) override;
        virtual bool ForList(Lexer &) override;
        virtual bool ForPair(Lexer &) override;
        virtual bool ForValue(Lexer &) override;
        virtual bool ForPrimitive(Lexer &) override;
        virtual bool ForNumber(Lexer &) override;
        virtual bool ForKeyWord(Lexer &) override;
        virtual bool ForInteger(Lexer &) override;
        virtual bool ForFloat(Lexer &) override;
        virtual bool ForString(Lexer &) override;
        virtual bool ForError(Lexer &) override;
        virtual bool ForKey(Lexer &) override;
};

#endif