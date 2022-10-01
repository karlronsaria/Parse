#pragma once
#ifndef _JSONTREE_H
#define _JSONTREE_H

#include "Homonumeric.h"
#include <memory>
#include <string>
#include <vector>

namespace Json {
    template <typename T>
    class Tree;

    // Postorder-traversal visitor
    template <typename T>
    class ITreeVisitor {
        public:
            virtual ~ITreeVisitor() = default;
            virtual T ForString(const std::string &) = 0;
            virtual T ForNumeric(Homonumeric) = 0;
            virtual T ForObject(
                const std::vector<std::string> &,
                std::vector<T> &&
            ) = 0;
            virtual T ForList(std::vector<T> &&) = 0;
    };

    template <typename T>
    class Tree {
        public:
            virtual ~Tree() = default;
            virtual T Accept(std::shared_ptr<ITreeVisitor<T>>) = 0;
    };

    #ifdef TREE_T
    #undef TREE_T
    #endif
    #define TREE_T(T) std::unique_ptr<Json::Tree<T>>

    template <typename T>
    class Object: public Tree<T> {
        private:
            std::vector<std::string> _keys;
            std::vector<TREE_T(T)> _values;
        public:
            virtual ~Object() = default;

            Object(
                std::vector<std::string> keys,
                std::vector<TREE_T(T)> values
            ):  _keys(std::move(keys)),
                _values(std::move(values)) {}

            virtual T Accept(std::shared_ptr<ITreeVisitor<T>>) override;
    };

    template <typename T>
    class List: public Tree<T> {
        private:
            std::vector<TREE_T(T)> _values;
        public:
            virtual ~List() = default;

            List(std::vector<TREE_T(T)> values):
                _values(std::move(values)) {}

            virtual T Accept(std::shared_ptr<ITreeVisitor<T>>) override;
    };

    template <typename T>
    class String: public Tree<T> {
        private:
            std::string _payload;
        public:
            virtual ~String() = default;

            String(const std::string & payload):
                _payload(payload) {}

            virtual T Accept(std::shared_ptr<ITreeVisitor<T>>) override;
    };

    template <typename T>
    class Numeric: public Tree<T> {
        private:
            Homonumeric _payload;
        public:
            virtual ~Numeric() = default;

            Numeric(Homonumeric payload):
                _payload(payload) {}

            virtual T Accept(std::shared_ptr<ITreeVisitor<T>>) override;
    };
};

template <typename T>
T Json::Object<T>::Accept(std::shared_ptr<ITreeVisitor<T>> visitor) {
    std::vector<T> postvalues;

    for (auto & value : _values)
        postvalues.push_back(std::move(
            value->Accept(visitor)
        ));

    return visitor->ForObject(
        _keys,
        std::move(postvalues)
    );
}

template <typename T>
T Json::List<T>::Accept(std::shared_ptr<ITreeVisitor<T>> visitor) {
    std::vector<T> postvalues;

    for (auto & value : _values)
        postvalues.push_back(std::move(value->Accept(visitor)));

    return visitor->ForList(std::move(postvalues));
}

template <typename T>
T Json::String<T>::Accept(std::shared_ptr<ITreeVisitor<T>> visitor) {
    return visitor->ForString(_payload);
}

template <typename T>
T Json::Numeric<T>::Accept(std::shared_ptr<ITreeVisitor<T>> visitor) {
    return visitor->ForNumeric(_payload);
}

#undef TREE_T
#endif
