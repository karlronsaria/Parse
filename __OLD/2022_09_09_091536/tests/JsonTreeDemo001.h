#pragma once
#ifndef _JSONTREEDEMO001_H
#define _JSONTREEDEMO001_H

/*
#include "../src/JsonTree.h"
#include <iostream>
#include <iomanip>

class MyJsonTreeVisitor: public IJsonTreeVisitor {
    public:
        static const int STARTING_LEVEL = 0;
        static const int INCREMENT = 2;
    private:
        std::ostream & _stream;
        int _level;
    public:
        std::ostream & PutIndent();
        int Level() const;

        MyJsonTreeVisitor(std::ostream & stream):
            _stream(stream),
            _level(STARTING_LEVEL) {}

        virtual void Push();
        virtual void Pop();

        virtual void StartObject() override;
        virtual void EndObject() override;

        virtual void StartList() override;
        virtual void EndList() override;

        virtual void StartPair(const std::string &) override;
        virtual void EndPair() override;

        virtual void StartValue() override;
        virtual void EndValue() override;

        virtual void ForString(const std::string &) override;
        virtual void ForNumeric(Homonumeric) override;
};
*/

#endif