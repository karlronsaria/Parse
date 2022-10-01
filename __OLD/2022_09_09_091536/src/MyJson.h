#pragma once
#ifndef _MYJSON_H
#define _MYJSON_H

#include "../lib/StreamEnumerator.h"
#include "../lib/Lexer.h"
#include "../lib/JsonParser.h"
#include "../lib/JsonTree.h"
#include "../lib/JsonMachine.h"
#include "../lib/JsonBuilder.h"
#include <iomanip>

namespace Json {
    class MyLexer: public Lexer {
        private:
            std::vector<int>
            _codes;

            std::vector<std::string>
            _tokens;
        public:
            MyLexer(std::shared_ptr<IEnumerator> &&);
            virtual ~MyLexer() override = default;
            virtual int NextToken() override;

            const std::vector<int> & Codes() const;
            const std::vector<std::string> & Tokens() const;
    };

    class MyTreeFactory: public ITreeFactory<Tree<Pointer>> {
        public:
            virtual ~MyTreeFactory() = default;

            virtual ptr_t NewObject(
                std::vector<std::string> keys,
                std::vector<ptr_t> values
            ) override;

            virtual ptr_t NewList(std::vector<ptr_t>) override;
            virtual ptr_t NewString(const std::string &) override;
            virtual ptr_t NewNumeric(Homonumeric) override;
    };

    class MyPostorderTreeVisitor: public ITreeVisitor<Pointer> {
        private:
            std::shared_ptr<Machine> _machine;
        public:
            MyPostorderTreeVisitor(
                std::shared_ptr<Machine> machine
            ):  _machine(machine) {}

            virtual ~MyPostorderTreeVisitor() = default;
            virtual Pointer ForString(const std::string &) override;
            virtual Pointer ForNumeric(Homonumeric) override;
            virtual Pointer ForObject(
                const std::vector<std::string> &,
                std::vector<Pointer> &&
            ) override;
            virtual Pointer ForList(std::vector<Pointer> &&) override;
    };

    std::shared_ptr<const Machine>
    GetMachine(std::istream & inputStream);

    std::shared_ptr<Machine>
    GetMutableMachine(std::istream & inputStream);

    void
    ParserMessageToStream(
        std::ostream & outStream,
        const MyLexer & lexer,
        const std::string & message
    );

    std::string
    ParserMessageToString(
        const MyLexer & lexer,
        const std::string & message
    );

    struct MyResultSet {
        bool Success;
        std::shared_ptr<Machine> Machine;
        std::string Message;
    };

    MyResultSet
    RunMyParser(
        std::istream & inputStream
    );
};

#endif