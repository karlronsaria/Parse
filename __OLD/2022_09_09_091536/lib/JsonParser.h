#pragma once
#ifndef _JSONPARSER_H
#define _JSONPARSER_H

#include "Homonumeric.h"
#include "Lexer.h"
#include <vector>

// **note: sfinae
//   link: https://en.wikipedia.org/wiki/Substitution_failure_is_not_an_error#:~:text=Substitution%20failure%20is%20not%20an%20error%20(SFINAE)%20refers%20to%20a,to%20describe%20related%20programming%20techniques.
//   retrieved: 2022_07_26

namespace Json {
    template <typename Tree_Type>
    class ITreeFactory {
        public:
            typedef std::unique_ptr<Tree_Type>
            ptr_t;

            virtual ~ITreeFactory() = default;

            virtual ptr_t NewObject(
                std::vector<std::string> keys,
                std::vector<ptr_t> values
            ) = 0;

            virtual ptr_t NewList(
                std::vector<ptr_t> values
            ) = 0;

            virtual ptr_t NewString(
                const std::string &
            ) = 0;

            virtual ptr_t NewNumeric(
                Homonumeric
            ) = 0;
    };

    class Parsing {
        public:
            static int Escape(::Lexer &);
            static int NextToken(::Lexer &);
    };

    class Lexer {
        private:
            ::Lexer _lexer;
        public:
            Lexer(std::shared_ptr<IEnumerator> &&);
            virtual ~Lexer() = default;
            virtual int NextToken();
            virtual const std::string & String() const;
            virtual int Integer() const;
            virtual float Float() const;
            virtual int Index() const;
    };

    template <typename Tree_Type>
    class Parser {
        public:
            typedef typename std::unique_ptr<Tree_Type>
            tree_t;

            typedef typename ITreeFactory<Tree_Type>::ptr_t
            ptr_t;

            struct ResultSet {
                bool Success;
                ptr_t Tree;
                std::string Message;
            };
        private:
            std::unique_ptr<ITreeFactory<Tree_Type>>
            _factory;

            std::shared_ptr<Lexer>
            _lexer;

            int
            _token;
        protected:
            ResultSet ParseObject();
            ResultSet ParseList();
            void ParsePair(std::string &, ResultSet &);
            ResultSet ParseValue();
            ResultSet ParsePrimitive();
            ResultSet ParseNumber(bool);
            ResultSet ParseKeyword();
            int NextToken();
            ResultSet Error(const std::string &);
            ResultSet Subtree(ptr_t &&);
        public:
            virtual ~Parser() = default;

            Parser(
                std::unique_ptr<ITreeFactory<Tree_Type>> factory,
                std::shared_ptr<Lexer> lexer
            ):  _factory(std::move(factory)),
                _lexer(lexer),
                _token(0) {}

            Parser(
                std::unique_ptr<ITreeFactory<Tree_Type>> factory
            ):  _factory(std::move(factory)),
                _lexer(std::make_shared<Lexer>()),
                _token(0) {}

            ResultSet GetTree();

            static ResultSet Tree(
                std::unique_ptr<ITreeFactory<Tree_Type>> factory,
                std::shared_ptr<Lexer> lexer
            );

            /* TODO: sfinae **see above */ \
            template <typename Factory_Type>
            static ResultSet Tree(
                std::shared_ptr<Lexer> lexer
            );

            /* TODO: sfinae **see above */ \
            template <template <class> class Factory_Type>
            static ResultSet Tree(
                std::shared_ptr<Lexer> lexer
            );
    };
};

template <typename T>
int
Json::Parser<T>::NextToken() {
    return _token = _lexer->NextToken();
}

template <typename T>
typename Json::Parser<T>::ResultSet
Json::Parser<T>::GetTree() {
    NextToken();

    if (_token != '{')
        // Error, gnostic
        return Error("Expected '{'");

    // Subtree, agnostic
    return ParseObject();
}

template <typename T>
typename Json::Parser<T>::ResultSet
Json::Parser<T>::ParseObject() {
    // [x] TODO: Consider adding a kill condition at the
    //   start of each Parse function.
    //   
    //   2022_07_28: If the code reaches this section,
    //   then no error has occurred, and there's no need
    //   for a kill condition.
    std::vector<std::string> keys;
    std::vector<tree_t> values;
    std::string key;
    ResultSet value;

    do {
        // Subtree, agnostic
        ParsePair(key, value);

        if (!value.Success)
            // Error, agnostic
            return value;

        keys.push_back(std::move(key));
        values.push_back(std::move(value.Tree));
        NextToken();
    }
    while (_token == ',');

    if (_token != '}')
        // Error, gnostic
        return Error("Expected '}'");

    // Subtree, gnostic
    return Subtree(
        std::move(
            _factory->NewObject(
                std::move(keys),
                std::move(values)
            )
        )
    );
}

template <typename T>
typename Json::Parser<T>::ResultSet
Json::Parser<T>::ParseList() {
    auto values = std::vector<tree_t>();

    do {
        // Subtree, agnostic
        auto value = ParseValue();

        if (_token == Token::ERROR)
            // Error, agnostic
            return value;

        values.push_back(std::move(value.Tree));
        NextToken();
    }
    while (_token == ',');

    if (_token != ']')
        // Error, gnostic
        return Error("Expected ']'");

    // Subtree, gnostic
    return Subtree(
        std::move(
            _factory->NewList(std::move(values))
        )
    );
}

template <typename T>
void
Json::Parser<T>::ParsePair(
    std::string & key,
    Json::Parser<T>::ResultSet & value
) {
    NextToken();

    if ((Token)_token != Token::STRING) {
        // Error, gnostic
        value = Error("Expected a string");
        return;
    }

    key = _lexer->String();
    NextToken();

    if ((char)_token != ':') {
        // Error, gnostic
        value = Error("Expected ':'");
        return;
    }

    // Subtree, agnostic
    value = std::move(ParseValue());
}

template <typename T>
typename Json::Parser<T>::ResultSet
Json::Parser<T>::ParseValue() {
    NextToken();

    if (_token == '[')
        // Subtree, agnostic
        return ParseList();

    if (_token == '{')
        // Subtree, agnostic
        return ParseObject();

    // Subtree, agnostic
    return ParsePrimitive();
}

template <typename T>
typename Json::Parser<T>::ResultSet
Json::Parser<T>::ParsePrimitive() {
    switch ((Token)_token) {
        case Token::WORD:
            // Subtree, agnostic
            return ParseKeyword();
        case Token::STRING:
            // Subtree, gnostic
            return Subtree(
                std::move(
                    _factory->NewString(_lexer->String())
                )
            );
        default:
            if ((char)_token == '-') {
                NextToken();
                // Subtree, agnostic
                return ParseNumber(true);
            }
    }

    // Subtree, agnostic
    return ParseNumber(false);
}

template <typename T>
typename Json::Parser<T>::ResultSet
Json::Parser<T>::ParseNumber(bool negative) {
    int factor = negative ? -1 : 1;

    switch ((Token)_token) {
        case Token::INTEGER:
            // Subtree, gnostic
            return Subtree(
                std::move(
                    _factory->NewNumeric(
                        Homonumeric::Integer(factor * _lexer->Integer())
                    )
                )
            );
        case Token::FLOAT:
            // Subtree, gnostic
            return Subtree(
                std::move(
                    _factory->NewNumeric(
                        Homonumeric::Float(factor * _lexer->Float())
                    )
                )
            );
        default:
            break;
    }

    // Error, gnostic
    return Error("Expected a number (integer, float, or boolean)");
}

template <typename T>
typename Json::Parser<T>::ResultSet
Json::Parser<T>::ParseKeyword() {
    std::string keyword = _lexer->String();

    if (keyword == "null")
        // Subtree, gnostic
        return Subtree(nullptr);

    int value = 0;

    if (keyword == "false")
        value = 1;
    else if (keyword == "true")
        value = 2;

    if (!value)
        // Error, gnostic
        return Error("Unexpected keyword '" + keyword + '\'');

    // Subtree, gnostic
    return Subtree(
        std::move(
            _factory->NewNumeric(
                Homonumeric::Boolean((bool)(value - 1))
            )
        )
    );
}

template <typename T>
typename Json::Parser<T>::ResultSet
Json::Parser<T>::Error(const std::string & message) {
    return ResultSet {
        false,
        nullptr,
        message
    };
}

template <typename T>
typename Json::Parser<T>::ResultSet
Json::Parser<T>::Subtree(ptr_t && tree) {
    return ResultSet {
        true,
        std::move(tree),
        ""
    };
}

template <typename T>
typename Json::Parser<T>::ResultSet
Json::Parser<T>::Tree(
    std::unique_ptr<ITreeFactory<T>> factory,
    std::shared_ptr<Lexer> lexer
) {
    return Parser(factory, lexer).GetTree();
}

template <typename T>
template <typename F>
typename Json::Parser<T>::ResultSet
Json::Parser<T>::Tree(
    std::shared_ptr<Lexer> lexer
) {
    return Parser<T>(
        std::make_unique<F>(),
        lexer
    )
    .GetTree();
}

template <typename T>
template <template <class> class F>
typename Json::Parser<T>::ResultSet
Json::Parser<T>::Tree(
    std::shared_ptr<Lexer> lexer
) {
    return Parser<T>(
        std::make_unique<F<T>>(),
        lexer
    )
    .GetTree();
}

#endif
