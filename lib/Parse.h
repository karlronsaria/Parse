#pragma once
#ifndef _PARSE_H_
#define _PARSE_H_

#include <string>
#include <memory>
#include <functional>

#ifdef __DIVIDEND__
#undef __DIVIDEND__
#endif

#if __cplusplus >= 202002L

template <typename T>
concept StringDivisible = requires (
    T d, bool b, char c, std::string s1, const char * s2
) {
    d.Success();
    d.Copy(b);
    d.operator!();
    d.operator/(c);
    d.operator/(s1);
    d.operator/(s2);
};

#define __DIVIDEND__ StringDivisible
#else
#define __DIVIDEND__ typename
#endif

template <__DIVIDEND__ T>
class AParse {
public:
    typedef std::function<T(T)> functor_t;
private:
    functor_t _action;
public:
    virtual ~AParse() = default;
    AParse() = default;

    AParse(functor_t action):
        _action(action) {}

    virtual functor_t
    Get() const {
        return _action;
    }

    const std::shared_ptr<AParse<T>>
    And(std::shared_ptr<AParse<T>> other) const {
        auto first = this->Get();
        auto secnd = other->Get();

        return std::make_shared<AParse<T>>(
            [=](T result_0) -> T {
                T result_1 = first(result_0);

                return !result_1.success
                    ? result_1
                    : secnd(result_1)
                    ;
            }
        );
    }

    const std::shared_ptr<AParse<T>>
    Or(std::shared_ptr<AParse<T>> other) const {
        auto first = this->Get();
        auto secnd = other->Get();

        return std::make_shared<AParse<T>>(
            [=](T result_0) -> T {
                T result_1 = first(result_0);

                return result_1.success
                    ? result_1
                    : secnd(result_0)
                    ;
            }
        );
    }
};

template <__DIVIDEND__ T>
class Parse: public AParse<T> {
private:
    typename AParse<T>::functor_t _action;
public:
    virtual ~Parse() = default;

    static std::shared_ptr<AParse<T>>
    New(typename AParse<T>::functor_t action) {
        return std::make_shared<Parse<T>>(action);
    }

    static std::shared_ptr<AParse<T>>
    New(char c) {
        return std::make_shared<Parse<T>>(c);
    }

    static std::shared_ptr<AParse<T>>
    New(bool decision) {
        return std::make_shared<Parse<T>>(decision);
    }

    static std::shared_ptr<AParse<T>>
    New(std::string str) {
        return std::make_shared<Parse<T>>(str);
    }

    Parse(typename AParse<T>::functor_t action):
        _action(action) {}

    Parse(const char * divisor):
        _action([divisor](T dividend) -> T {
            return dividend / divisor;
        }) {}

    Parse(std::string divisor):
        _action([divisor](T dividend) -> T {
            return dividend / divisor;
        }) {}

    Parse(std::function<bool(char)> condition, char divisor):
        _action([condition, divisor](T dividend) -> T {
            return condition(divisor)
                ? dividend.Copy(false)
                : dividend / divisor;
        }) {}

    Parse(char divisor):
        _action([divisor](T dividend) -> T {
            return dividend / divisor;
        }) {}

    Parse(bool decision):
        _action([decision](T result) -> T {
            return result.Copy(decision);
        }) {}

    virtual typename AParse<T>::functor_t
    Get() const override {
        return _action;
    }
};

template <__DIVIDEND__ T>
const std::shared_ptr<AParse<T>>
operator*(
    const std::shared_ptr<AParse<T>> first,
    const std::shared_ptr<AParse<T>> secnd
) {
    return first->And(secnd);
}

template <__DIVIDEND__ T>
const std::shared_ptr<AParse<T>>
operator+(
    const std::shared_ptr<AParse<T>> first,
    const std::shared_ptr<AParse<T>> secnd
) {
    return first->Or(secnd);
}

template <__DIVIDEND__ T>
const std::shared_ptr<AParse<T>>
operator*(
    const int scalar,
    const std::shared_ptr<AParse<T>> vector
) {
    auto temp = Parse<T>::New(true);

    for (int i = 1; i <= scalar; ++i)
        temp = temp->Or(vector);

    return temp;
}

template <__DIVIDEND__ T>
const std::shared_ptr<AParse<T>>
Pow(
    const std::shared_ptr<AParse<T>> vector,
    const int scalar
) {
    auto temp = Parse<T>::New(true);

    for (int i = 1; i <= scalar; ++i)
        temp = temp->And(vector);

    return temp;
}

// Seq ::= x Seq | nil
// parse_seq = (x * parse_seq) + parse_true

template <__DIVIDEND__ T>
const std::shared_ptr<AParse<T>>
While(
    const std::shared_ptr<AParse<T>> vector
) {
    return Parse<T>::New(typename AParse<T>::functor_t(
        [vector](T init) {
            T next = vector->Get()(init);

            auto nextParse = next.success
                ? While(vector)
                : Parse<T>::New(true)
                ;

            return nextParse->Get()(next);
        }
    ));
}

template <__DIVIDEND__ T>
class ParseEmpty: public AParse<T> {
public:
    virtual ~ParseEmpty() = default;

    static std::shared_ptr<AParse<T>>
    New() {
        return std::make_shared<ParseEmpty<T>>();
    }

    virtual typename AParse<T>::functor_t
    Get() const override {
        return [](T result) -> T {
            return result.Copy(result.remainder.length() == 0);
        };
    }
};

#endif