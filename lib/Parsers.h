#include "Parse.h"

struct Triple {
    bool success;
    std::string quotient;
    std::string remainder;

    bool Success() const {
        return success;
    }

    const Triple Copy(bool decision) const {
        return Triple {
            .success = decision,
            .quotient = quotient,
            .remainder = remainder,
        };
    }

    const Triple operator!() const {
        return Triple {
            .success = !success,
            .quotient = quotient,
            .remainder = remainder,
        };
    }

    const Triple Diff(std::function<bool(char)> condition) const {
        return remainder.length() == 0 || !condition(remainder[0])
            ? Copy(false)
            : Triple {
                .success = true,
                .quotient = std::string(quotient) + remainder[0],
                .remainder = remainder.substr(
                    1,
                    remainder.length() - 1
                ),
            };
    }

    const Triple By(char divisor) const {
        return remainder.length() == 0 || remainder[0] != divisor
            ? Copy(false)
            : Triple {
                .success = true,
                .quotient = std::string(quotient) + divisor,
                .remainder = remainder.substr(
                    1,
                    remainder.length() - 1
                ),
            };
    }

    const Triple By(const std::string & divisor) const {
        auto dividend = remainder;

        bool fail = (dividend.length() < divisor.length())
            || (dividend.substr(0, divisor.length()) != divisor);

        if (fail)
            return !*this;

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

    const Triple By(const char * divisor) const {
        return By(std::string(divisor));
    }

    const Triple operator/(char divisor) const { return By(divisor); }
    const Triple operator/(const std::string & divisor) const { return By(divisor); }
    const Triple operator/(const char * divisor) const { return By(divisor); }
};

struct Integral: public Triple {
private:
    static int Divide(int value, char c) {
        return (10 * value) + (int)(c - '0');
    }

    static int Divide(int value, const std::string & str) {
        for (char c : str)
            value = Divide(value, c);

        return value;
    }
public:
    int value;

    static Integral New(Triple base, int value) {
        Integral temp;
        temp.success = base.success;
        temp.quotient = base.quotient;
        temp.remainder = base.remainder;
        temp.value = value;
        return temp;
    }

    const Integral Copy(bool decision) const {
        Triple t = Triple::Copy(decision);
        return New(t, value);
    }

    const Integral Diff(std::function<bool(char)> condition) const {
        Triple t = Triple::Diff(condition);

        int i = t.success
            ? Divide(value, t.quotient[t.quotient.length() - 1])
            : value
            ;

        return New(t, i);
    }

    const Integral operator!() const {
        Triple t = Triple::operator!();
        return New(t, value);
    }

    const Integral operator/(char divisor) const {
        Triple t = Triple::operator/(divisor);

        int i = t.success
            ? Divide(value, divisor)
            : value
            ;

        return New(t, i);
    }

    const Integral operator/(const std::string & divisor) const {
        Triple t = Triple::operator/(divisor);

        int i = t.success
            ? Divide(value, divisor)
            : value
            ;

        return New(t, i);
    }

    const Integral operator/(const char * divisor) const {
        return *this / std::string(divisor);
    }
};

namespace Parsers {
    template <__DIVIDEND__ T>
    std::shared_ptr<AParse<T, T>>
    Digit() {
        return Parse<T, T>::New((std::function<bool(char)>)isdigit);
    }

    template <__DIVIDEND__ T>
    std::shared_ptr<AParse<T, T>>
    Alpha() {
        return Parse<T, T>::New((std::function<bool(char)>)isalpha);
    }

    template <__DIVIDEND__ T>
    std::shared_ptr<AParse<T, T>>
    Space() {
        return Parse<T, T>::New((std::function<bool(char)>)isspace);
    }

    template <__DIVIDEND__ T>
    std::shared_ptr<AParse<T, T>>
    Punct() {
        return Parse<T, T>::New((std::function<bool(char)>)ispunct);
    }

    template <__DIVIDEND__ T>
    std::shared_ptr<AParse<T, T>>
    Lower() {
        return Parse<T, T>::New((std::function<bool(char)>)islower);
    }

    template <__DIVIDEND__ T>
    std::shared_ptr<AParse<T, T>>
    Integer() {
        auto digit = Digit<T>();
        return (And(-Parse<T, T>::New('0'), digit) * While(digit)) + digit;
    }

    template <__DIVIDEND__ T>
    std::shared_ptr<AParse<T, T>>
    Word() {
        auto word = Parse<T, T>::New('_') + Alpha<T>();
        return word * While<T, T>(word + Digit<T>());
    }
};
