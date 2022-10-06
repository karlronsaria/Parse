#include "Parse.h"
#include "Converts.h"

struct Integral: public Dividend<int> {
private:
    static int
    Divide(int value, char c) {
        return (10 * value) + (int)(c - '0');
    }

    static int
    Divide(int value, const std::string & str) {
        for (char c : str)
            value = Divide(value, c);

        return value;
    }
public:
    static Integral
    New(::Triple base, int value) {
        Integral temp;
        temp.success = base.success;
        temp.quotient = base.quotient;
        temp.remainder = base.remainder;
        temp.value = value;
        return temp;
    }

    const Integral
    Decide(bool decision) const {
        return New(Dividend<int>::Decide(decision), value);
    }

    const Integral
    Diff(std::function<bool(char)> condition) const {
        ::Triple t = Dividend<int>::Diff(condition);

        int i = t.success
            ? Divide(value, t.quotient[t.quotient.length() - 1])
            : value
            ;

        return New(t, i);
    }

    const Integral
    operator/(char divisor) const {
        ::Triple t = Dividend<int>::operator/(divisor);

        int i = t.success
            ? Divide(value, divisor)
            : value
            ;

        return New(t, i);
    }

    const Integral
    operator/(const std::string & divisor) const {
        ::Triple t = Dividend<int>::operator/(divisor);

        int i = t.success
            ? Divide(value, divisor)
            : value
            ;

        return New(t, i);
    }

    const Integral
    operator/(const char * divisor) const {
        return *this / std::string(divisor);
    }
};

namespace Parsers {
    template <__DIVIDEND__ T>
    std::shared_ptr<AParse<T, T>>
    Digit() {
        return Parse<T, T>::Satisfy(isdigit);
    }

    template <__DIVIDEND__ T>
    std::shared_ptr<AParse<T, T>>
    Alpha() {
        return Parse<T, T>::Satisfy(isalpha);
    }

    template <__DIVIDEND__ T>
    std::shared_ptr<AParse<T, T>>
    Space() {
        return Parse<T, T>::Satisfy(isspace);
    }

    template <__DIVIDEND__ T>
    std::shared_ptr<AParse<T, T>>
    Punct() {
        return Parse<T, T>::Satisfy(ispunct);
    }

    template <__DIVIDEND__ T>
    std::shared_ptr<AParse<T, T>>
    Lower() {
        return Parse<T, T>::Satisfy(islower);
    }

    template <__DIVIDEND__ T>
    std::shared_ptr<AParse<T, T>>
    Natural() {
        auto digit = Digit<T>();
        return (And(-Parse<T, T>::Char('0'), digit) * Some(digit)) + digit;
    }

    template <__DIVIDEND__ T>
    std::shared_ptr<AParse<T, T>>
    Word() {
        auto word = Parse<T, T>::Char('_') + Alpha<T>();
        return word * Some<T, T>(word + Digit<T>());
    }

    std::shared_ptr<AParse<Integral, Dividend<float>>>
    Float() {
        auto has_point =
            Parse<Integral, Integral>::Functor(
                [](Integral i) -> Integral {
                    return Integral::New(
                        Parse<Triple, Triple>::Char('.')->Fn()(i),
                        i.value
                    );
                }
            );

        auto has_tail =
            Parse<Integral, Dividend<float>>::Functor(
                [](Integral i) -> Dividend<float> {
                    Integral i2 = Parsers::Natural<Integral>()->Fn()(
                        Integral::New(i.Triple(), 0)
                    );

                    float f1 = Converts::ToFloat(i.value, i2.value);
                    return Dividend<float>::New(i2.Triple(), f1);
                }
            );

        return has_point * has_tail;
    }
};
