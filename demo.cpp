#include <iostream>
#include <string>
#include <memory>
#include <functional>

class Tests {
public:
    static std::string WorkingDirectory;

    static void Init() {
    }

    static void Run(std::ostream & out) {
    }
};

std::string Tests::WorkingDirectory;

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

// #define RUN_TESTS

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

    const Triple By(std::string divisor) const {
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
    const Triple operator/(std::string divisor) const { return By(divisor); }
    const Triple operator/(const char * divisor) const { return By(divisor); }
};

template <__DIVIDEND__ T>
class AParser;

// typedef std::shared_ptr<AParser> parser_t;

/*
template <class T, class... Args>
parser_t ptr(Args &&... args) {
    return std::make_shared<T>(args...);
}

template <class T>
parser_t ptr() {
    return std::make_shared<T>();
}
*/

template <__DIVIDEND__ T>
class AParser {
public:
    typedef std::function<T(T)> functor_t;
private:
    functor_t _action;
public:
    virtual ~AParser() = default;
    AParser() = default;

    AParser(functor_t action):
        _action(action) {}

    virtual functor_t
    Get() const {
        return _action;
    }

    const std::shared_ptr<AParser<T>>
    And(std::shared_ptr<AParser<T>> other) const {
        auto first = this->Get();
        auto secnd = other->Get();

        return std::make_shared<AParser<T>>(
            [=](T result_0) -> T {
                T result_1 = first(result_0);

                return !result_1.success
                    ? result_1
                    : secnd(result_1)
                    ;
            }
        );
    }

    const std::shared_ptr<AParser<T>>
    Or(std::shared_ptr<AParser<T>> other) const {
        auto first = this->Get();
        auto secnd = other->Get();

        return std::make_shared<AParser<T>>(
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
class Parse: public AParser<T> {
private:
    typename AParser<T>::functor_t _action;
public:
    virtual ~Parse() = default;

    static std::shared_ptr<AParser<T>>
    New(typename AParser<T>::functor_t action) {
        return std::make_shared<Parse<T>>(action);
    }

    static std::shared_ptr<AParser<T>>
    New(char c) {
        return std::make_shared<Parse<T>>(c);
    }

    static std::shared_ptr<AParser<T>>
    New(bool decision) {
        return std::make_shared<Parse<T>>(decision);
    }

    static std::shared_ptr<AParser<T>>
    New(std::string str) {
        return std::make_shared<Parse<T>>(str);
    }

    Parse(typename AParser<T>::functor_t action):
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

    virtual typename AParser<T>::functor_t
    Get() const override {
        return _action;
    }
};

template <__DIVIDEND__ T>
const std::shared_ptr<AParser<T>>
operator*(
    const std::shared_ptr<AParser<T>> first,
    const std::shared_ptr<AParser<T>> secnd
) {
    return first->And(secnd);
}

template <__DIVIDEND__ T>
const std::shared_ptr<AParser<T>>
operator+(
    const std::shared_ptr<AParser<T>> first,
    const std::shared_ptr<AParser<T>> secnd
) {
    return first->Or(secnd);
}

template <__DIVIDEND__ T>
const std::shared_ptr<AParser<T>>
operator*(
    const int scalar,
    const std::shared_ptr<AParser<T>> vector
) {
    auto temp = Parse<T>::New(true);

    for (int i = 1; i <= scalar; ++i)
        temp = temp->Or(vector);

    return temp;
}

template <__DIVIDEND__ T>
const std::shared_ptr<AParser<T>>
Pow(
    const std::shared_ptr<AParser<T>> vector,
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
const std::shared_ptr<AParser<T>>
While(
    const std::shared_ptr<AParser<T>> vector
) {
    return Parse<T>::New(typename AParser<T>::functor_t(
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
class ParseEmpty: public AParser<T> {
public:
    virtual ~ParseEmpty() = default;

    static std::shared_ptr<AParser<T>>
    New() {
        return std::make_shared<ParseEmpty<T>>();
    }

    virtual typename AParser<T>::functor_t
    Get() const override {
        return [](T result) -> T {
            return result.Copy(result.remainder.length() == 0);
        };
    }
};

int main(int argc, char ** args) {
    if (argc <= 1) {
        std::cout
            << "This demo requires a working directory as a command-line argument.\n";

        return 1;
    }

    Tests::WorkingDirectory = std::string(args[1]);

    #ifdef RUN_TESTS
    {
        Tests::Init();
        Tests::Run(std::cout);
    }
    #else
    {
        auto res = Triple {
            .success = true,
            .quotient = "",
            .remainder = "what",
        };

        auto parse_w = Parse<Triple>::New('w');
        auto parse_h = Parse<Triple>::New('h');
        auto parse_a = Parse<Triple>::New('a');
        auto parse_t = Parse<Triple>::New('t');
        auto parse_empty = ParseEmpty<Triple>::New();

        auto parse_what =
            parse_w
            ->And(parse_h)
            ->And(parse_a)
            ->And(parse_t)
            ->And(parse_empty)
            ;

        auto final = parse_what->Get()(res);

        if (final.success)
            std::cout
                << "Quotient  : " << final.quotient << '\n'
                << "Remainder : " << final.remainder << '\n'
                ;

        auto parse_hello =
            Parse<Triple>::New('h')
          * Parse<Triple>::New('e')
          * (Pow(Parse<Triple>::New('l'), 2))
          * Parse<Triple>::New('o')
            ;

        auto parse_world =
            Parse<Triple>::New(std::string("world"))
            ;

        final = (
            parse_hello
          * Parse<Triple>::New(' ')
          * parse_world
          * ParseEmpty<Triple>::New()
        )->Get()(
            Triple {
                .quotient = "",
                .remainder = "hello world"
            }
        );

        if (!final.success)
            std::cout << "Fail:\n";

        std::cout
            << "Quotient  : " << final.quotient << '\n'
            << "Remainder : " << final.remainder << '\n'
            ;

        auto parse_while_x = While(Parse<Triple>::New('x'));

        final = parse_while_x->Get()(
            Triple {
                .quotient = "",
                .remainder = "xxxxxxxaaaa"
            }
        );

        if (!final.success)
            std::cout << "Fail:\n";

        std::cout
            << "Quotient  : " << final.quotient << '\n'
            << "Remainder : " << final.remainder << '\n'
            ;
    }
    #endif

    return 0;
}
