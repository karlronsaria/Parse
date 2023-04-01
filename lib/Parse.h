#pragma once
#ifndef _PARSE_H_
#define _PARSE_H_

#include <string>
#include <memory>
#include <functional>

#ifdef lambda
#undef lambda
#endif
#define lambda(a, b) [](auto a) -> auto { return b; }

struct Triple {
    bool success;
    std::string quotient;
    std::string remainder;

    static const Triple
    New(const Triple other) {
        return other.Decide(other.Success());
    }

    const Triple
    GetTriple() const {
        return *this;
    }

    bool
    Success() const {
        return success;
    }

    const Triple
    Decide(bool decision) const {
        return Triple {
            .success = decision,
            .quotient = quotient,
            .remainder = remainder,
        };
    }

    const Triple
    Discard(const std::string & quotient) const {
        return Triple {
            .success = this.success,
            .quotient = quotient,
            .remainder = this.remainder,
        }
    }

    const Triple
    Diff() const {
        return remainder.length() == 0
            ? Decide(false)
            : Triple {
                .success = true,
                .quotient = std::string(quotient) + remainder[0],
                .remainder = remainder.substr(
                    1,
                    remainder.length() - 1
                ),
            };
    }

    const Triple
    Diff(std::function<bool(char)> condition) const {
        return remainder.length() == 0 || !condition(remainder[0])
            ? Decide(false)
            : Triple {
                .success = true,
                .quotient = std::string(quotient) + remainder[0],
                .remainder = remainder.substr(
                    1,
                    remainder.length() - 1
                ),
            };
    }

    const Triple
    operator/(char divisor) const {
        return remainder.length() == 0 || remainder[0] != divisor
            ? Decide(false)
            : Triple {
                .success = true,
                .quotient = std::string(quotient) + divisor,
                .remainder = remainder.substr(
                    1,
                    remainder.length() - 1
                ),
            };
    }

    const Triple
    operator/(const std::string & divisor) const {
        auto dividend = remainder;

        bool fail = (dividend.length() < divisor.length())
            || (dividend.substr(0, divisor.length()) != divisor);

        if (fail)
            return this->Decide(false);

        auto remainder = dividend.substr(
            divisor.length(),
            dividend.length() - divisor.length()
        );

        return Triple {
            .success = true,
            .quotient = std::string(quotient) + std::string(divisor),
            .remainder = remainder,
        };
    }

    const Triple
    operator/(const char * divisor) const {
        return (*this)/(std::string(divisor));
    }
};

template <typename T>
struct Dividend: public Triple {
public:
    T value;

    static Dividend<T>
    New(Triple base, T value) {
        Dividend<T> temp;
        temp.success = base.success;
        temp.quotient = base.quotient;
        temp.remainder = base.remainder;
        temp.value = value;
        return temp;
    }

    static Dividend<T>
    New(Triple base) {
        return New(base, 0);
    }

    const Triple
    GetTriple() const {
        return ::Triple {
            .success = success,
            .quotient = quotient,
            .remainder = remainder,
        };
    }

    bool
    Success() const {
        return Triple::Success();
    }

    const Dividend<T>
    Decide(bool decision) const {
        return New(Triple::Decide(decision), value);
    }

    const Dividend<T>
    Diff() const {
        return New(Triple::Diff(), value);
    }

    const Dividend<T>
    Diff(std::function<bool(char)> condition) const {
        return New(Triple::Diff(condition), value);
    }

    const Dividend<T>
    operator/(char divisor) const {
        return New(Triple::operator/(divisor), value);
    }

    const Dividend<T>
    operator/(const std::string & divisor) const {
        return New(Triple::operator/(divisor), value);
    }

    const Dividend<T>
    operator/(const char * divisor) const {
        return *this / std::string(divisor);
    }
};

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
    d = d.Decide(b);
    d = d.Diff(f);
    d = d.Diff();
    d = d.operator/(c);
    d = d.operator/(s1);
    d = d.operator/(s2);
    d = T::New(d.GetTriple());
};

#define __DIVIDEND__ StringDivisible
#else
#define __DIVIDEND__ typename
#endif

template <__DIVIDEND__ T, __DIVIDEND__ S>
class AParse {
public:
    typedef std::function<S(T)> functor_t;
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

    const std::shared_ptr<AParse<T, S>>
    Or(std::shared_ptr<AParse<T, S>> other) const {
        auto first = this->Fn();
        auto secnd = other->Fn();

        return std::make_shared<AParse<T, S>>(
            [=](T result_0) -> S {
                S result_1 = first(result_0);

                return result_1.Success()
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
                S temp = first(result_0);
                return temp.Decide(!temp.Success());
            }
        );
    }

    const std::shared_ptr<AParse<T, S>>
    operator-() const {
        return Not();
    }
};

template <__DIVIDEND__ T, __DIVIDEND__ S, __DIVIDEND__ R>
const std::shared_ptr<AParse<T, R>>
Cat(
    std::shared_ptr<AParse<T, S>> first,
    std::shared_ptr<AParse<S, R>> secnd
) {
    auto f1 = first->Fn();
    auto f2 = secnd->Fn();

    return std::make_shared<AParse<T, R>>(
        [=](T result_0) -> R {
            S result_1 = f1(result_0);

            return !result_1.Success()
                ? R::New(result_1.GetTriple()) // note: SFINAE
                : f2(result_1)
                ;
        }
    );
}

template <__DIVIDEND__ T>
const std::shared_ptr<AParse<T, T>>
Cat(
    std::shared_ptr<AParse<T, T>> first,
    std::shared_ptr<AParse<T, T>> secnd
) {
    auto f1 = first->Fn();
    auto f2 = secnd->Fn();

    return std::make_shared<AParse<T, T>>(
        [=](T result_0) -> T {
            T result_1 = f1(result_0);

            return !result_1.Success()
                ? result_1 // note: SFINAE
                : f2(result_1)
                ;
        }
    );
}

template <__DIVIDEND__ T, __DIVIDEND__ S>
class Parse: public AParse<T, S> {
private:
    typename AParse<T, S>::functor_t _action;
public:
    virtual ~Parse() = default;

    static std::shared_ptr<AParse<T, S>>
    Functor(typename AParse<T, S>::functor_t action) {
        return std::make_shared<Parse<T, S>>(action);
    }

    static std::shared_ptr<AParse<T, S>>
    Char(char c) {
        return std::make_shared<Parse<T, S>>(c);
    }

    static std::shared_ptr<AParse<T, S>>
    Decision(bool decision) {
        return std::make_shared<Parse<T, S>>(decision);
    }

    static std::shared_ptr<AParse<T, S>>
    String(std::string str) {
        return std::make_shared<Parse<T, S>>(str);
    }

    static std::shared_ptr<AParse<T, S>>
    Satisfy(std::function<bool(char)> condition) {
        return std::make_shared<Parse<T, S>>(condition);
    }

    static std::shared_ptr<AParse<T, S>>
    Any() {
        return std::make_shared<Parse<T, S>>();
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
            return result.Decide(decision);
        }) {}

    Parse():
        _action([](T result) -> S {
            return result.Diff();
        }) {}

    virtual typename AParse<T, S>::functor_t
    Fn() const override {
        return _action;
    }
};

template <__DIVIDEND__ T, __DIVIDEND__ S, __DIVIDEND__ R>
const std::shared_ptr<AParse<T, R>>
operator*(
    std::shared_ptr<AParse<T, S>> first,
    std::shared_ptr<AParse<S, R>> secnd
) {
    return Cat<T, S, R>(first, secnd);
}

template <__DIVIDEND__ T>
const std::shared_ptr<AParse<T, T>>
operator*(
    std::shared_ptr<AParse<T, T>> first,
    std::shared_ptr<AParse<T, T>> secnd
) {
    return Cat<T>(first, secnd);
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
    auto temp = Parse<T, T>::Decision(true);

    for (int i = 1; i <= scalar; ++i)
        temp = temp * vector;

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
Some(
    const std::shared_ptr<AParse<T, T>> vector
) {
    return Parse<T, T>::Functor(
        [vector](T init) {
            T next = vector->Fn()(init);

            auto nextParse = next.Success()
                ? Some(vector)
                : Parse<T, T>::Decision(true)
                ;

            return nextParse->Fn()(next);
        }
    );
}

template <__DIVIDEND__ T>
const std::shared_ptr<AParse<T, T>>
Many(
    const std::shared_ptr<AParse<T, T>> vector
) {
    return vector * Some(vector);
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
            return result.Decide(result.remainder.length() == 0);
        };
    }
};

#endif
