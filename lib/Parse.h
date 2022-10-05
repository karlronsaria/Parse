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
    T d, bool b, char c, std::string s1, const char * s2,
    std::function<bool(char)> f
) {
    b = d.Success();
    d = d.Copy(b);
    d = d.Diff(f);
    d = d.operator!();
    d = d.operator/(c);
    d = d.operator/(s1);
    d = d.operator/(s2);
};

#define __DIVIDEND__ StringDivisible
#else
#define __DIVIDEND__ typename
#endif

template <__DIVIDEND__ T, __DIVIDEND__ S>
class AParse {
public:
    typedef std::function<T(S)> functor_t;
private:
    functor_t _action;
public:
    virtual ~AParse() = default;
    AParse() = default;

    AParse(functor_t action):
        _action(action) {}

    virtual functor_t
    Fn() const {
        return _action;
    }

    template <__DIVIDEND__ R>
    const std::shared_ptr<AParse<T, R>>
    Cat(std::shared_ptr<AParse<S, R>> other) const {
        auto first = this->Fn();
        auto secnd = other->Fn();

        return std::make_shared<AParse<T, R>>(
            [=](T result_0) -> R {
                S result_1 = first(result_0);

                return !result_1.success
                    ? result_1
                    : secnd(result_1)
                    ;
            }
        );
    }

    const std::shared_ptr<AParse<T, S>>
    Or(std::shared_ptr<AParse<T, S>> other) const {
        auto first = this->Fn();
        auto secnd = other->Fn();

        return std::make_shared<AParse<T, S>>(
            [=](T result_0) -> S {
                S result_1 = first(result_0);

                return result_1.success
                    ? result_1
                    : secnd(result_0)
                    ;
            }
        );
    }

    const std::shared_ptr<AParse<T, S>>
    Not() const {
        auto first = this->Fn();

        return std::make_shared<AParse<T, S>>(
            [=](T result_0) -> S {
                return !first(result_0);
            }
        );
    }

    const std::shared_ptr<AParse<T, S>>
    operator-() const {
        return Not();
    }
};

template <__DIVIDEND__ T, __DIVIDEND__ S>
class Parse: public AParse<T, S> {
private:
    typename AParse<T, S>::functor_t _action;
public:
    virtual ~Parse() = default;

    static std::shared_ptr<AParse<T, S>>
    New(typename AParse<T, S>::functor_t action) {
        return std::make_shared<Parse<T, S>>(action);
    }

    static std::shared_ptr<AParse<T, S>>
    New(char c) {
        return std::make_shared<Parse<T, S>>(c);
    }

    static std::shared_ptr<AParse<T, S>>
    New(bool decision) {
        return std::make_shared<Parse<T, S>>(decision);
    }

    static std::shared_ptr<AParse<T, S>>
    New(std::string str) {
        return std::make_shared<Parse<T, S>>(str);
    }

    static std::shared_ptr<AParse<T, S>>
    New(std::function<bool(char)> condition) {
        return std::make_shared<Parse<T, S>>(condition);
    }

    Parse(typename AParse<T, S>::functor_t action):
        _action(action) {}

    Parse(const char * divisor):
        _action([divisor](T dividend) -> S {
            return dividend / divisor;
        }) {}

    Parse(std::string divisor):
        _action([divisor](T dividend) -> S {
            return dividend / divisor;
        }) {}

    Parse(std::function<bool(char)> condition):
        _action([condition](T dividend) -> S {
            return dividend.Diff(condition);
        }) {}

    Parse(char divisor):
        _action([divisor](T dividend) -> S {
            return dividend / divisor;
        }) {}

    Parse(bool decision):
        _action([decision](T result) -> S {
            return result.Copy(decision);
        }) {}

    virtual typename AParse<T, S>::functor_t
    Fn() const override {
        return _action;
    }
};

template <__DIVIDEND__ T, __DIVIDEND__ S, __DIVIDEND__ R>
const std::shared_ptr<AParse<T, R>>
operator*(
    const std::shared_ptr<AParse<T, S>> first,
    const std::shared_ptr<AParse<S, R>> secnd
) {
    return first->Cat(secnd);
}

template <__DIVIDEND__ T, __DIVIDEND__ S>
const std::shared_ptr<AParse<T, S>>
operator+(
    const std::shared_ptr<AParse<T, S>> first,
    const std::shared_ptr<AParse<T, S>> secnd
) {
    return first->Or(secnd);
}

template <__DIVIDEND__ T, __DIVIDEND__ S>
const std::shared_ptr<AParse<T, S>>
operator-(
    const std::shared_ptr<AParse<T, S>> first,
    const std::shared_ptr<AParse<T, S>> secnd
) {
    return first->Or(secnd->Not());
}

template <__DIVIDEND__ T>
const std::shared_ptr<AParse<T, T>>
Pow(
    const std::shared_ptr<AParse<T, T>> vector,
    const int scalar
) {
    auto temp = Parse<T, T>::New(true);

    for (int i = 1; i <= scalar; ++i)
        temp = temp->Cat(vector);

    return temp;
}

template <__DIVIDEND__ T, __DIVIDEND__ S>
const std::shared_ptr<AParse<T, S>>
operator-(const std::shared_ptr<AParse<T, S>> vector) {
    return vector->Not();
}

template <__DIVIDEND__ T, __DIVIDEND__ S>
const std::shared_ptr<AParse<T, S>>
And(
    const std::shared_ptr<AParse<T, S>> a,
    const std::shared_ptr<AParse<T, S>> b
) {
    return -(-a - b);
}

// Seq ::= x Seq | nil
// parse_seq = (x * parse_seq) + parse_true

template <__DIVIDEND__ T>
const std::shared_ptr<AParse<T, T>>
While(
    const std::shared_ptr<AParse<T, T>> vector
) {
    return Parse<T, T>::New(typename AParse<T, T>::functor_t(
        [vector](T init) {
            T next = vector->Fn()(init);

            auto nextParse = next.success
                ? While(vector)
                : Parse<T, T>::New(true)
                ;

            return nextParse->Fn()(next);
        }
    ));
}

template <__DIVIDEND__ T>
class ParseEmpty: public AParse<T, T> {
public:
    virtual ~ParseEmpty() = default;

    static std::shared_ptr<AParse<T, T>>
    New() {
        return std::make_shared<ParseEmpty<T>>();
    }

    virtual typename AParse<T, T>::functor_t
    Fn() const override {
        return [](T result) -> T {
            return result.Copy(result.remainder.length() == 0);
        };
    }
};

#endif