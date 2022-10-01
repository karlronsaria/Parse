#pragma once
#ifndef _JSONBUILDER_H
#define _JSONBUILDER_H

#include "JsonMachine.h"
#include <stack>

namespace Json {
    class Builder {
        public:
            enum Symbol {
                OBJECT,
                LIST,
                PAIR,
                VALUE,
                OTHER
            };
        private:
            std::stack<Symbol>
            _symbols;

            std::stack<Pointer>
            _pointers;

            std::stack<std::string>
            _keys;

            std::shared_ptr<Machine>
            _machine;

            void PushBack(Pointer);
            void SetValue(Pointer);

            void PushStack(Symbol, Pointer);
            void PopStack();

            bool AddValueToMachine(Pointer);
        public:
            Builder();
            Builder(std::shared_ptr<Machine> machine);

            virtual ~Builder() = default;
            Builder(const Builder &) = default;
            Builder & operator=(const Builder &) = default;

            Builder & Object();
            Builder & List();

            Builder & Pair(const std::string & key);
            Builder & Value();
            Builder & Pop();

            Builder & String(const std::string & value);
            Builder & Integer(int value);
            Builder & Float(float value);
            Builder & Boolean(bool value);

            std::shared_ptr<const Machine> Machine() const;
    };
};

#endif
