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

// #define RUN_TESTS

struct Triple {
    bool success;
    std::string quotient;
    std::string remainder;

    const Triple operator!() const {
        return Triple {
            .success = !success,
            .quotient = quotient,
            .remainder = remainder,
        };
    }

    const Triple Copy(bool decision) {
        return Triple {
            .success = decision,
            .quotient = quotient,
            .remainder = remainder,
        };
    }
};

typedef std::function<Triple(Triple)> functor_t;

class AParser;

typedef std::shared_ptr<AParser> parser_t;

template <class T, class... Args>
parser_t ptr(Args &&... args) {
    return std::make_shared<T>(args...);
}

template <class T>
parser_t ptr() {
    return std::make_shared<T>();
}

class AParser {
private:
    functor_t _action;
public:
    virtual ~AParser() = default;
    AParser() = default;

    AParser(functor_t action):
        _action(action) {}

    virtual functor_t Get() const {
        return _action;
    }

    const parser_t
    And(parser_t other) const {
        auto first = this->Get();
        auto secnd = other->Get();

        return ptr<AParser>(
            [=](Triple result_0) -> Triple {
                Triple result_1 = first(result_0);

                return !result_1.success
                    ? result_1
                    : secnd(result_1)
                    ;
            }
        );
    }

    const parser_t
    Or(parser_t other) const {
        auto first = this->Get();
        auto secnd = other->Get();

        return ptr<AParser>(
            [=](Triple result_0) -> Triple {
                Triple result_1 = first(result_0);

                return result_1.success
                    ? result_1
                    : secnd(result_0)
                    ;
            }
        );
    }
};

class Parse: public AParser {
private:
    functor_t _action;
public:
    virtual ~Parse() = default;

    static parser_t New(functor_t action) { return ::ptr<Parse>(action); }
    static parser_t New(char c) { return ::ptr<Parse>(c); }
    static parser_t New(bool decision) { return ::ptr<Parse>(decision); }
    static parser_t New(std::string str) { return ::ptr<Parse>(str); }

    Parse(functor_t action):
        _action(action) {}

    Parse(const char * needle):
        Parse(std::string(needle)) {}

    Parse(std::string needle):
        _action([needle](Triple result) -> Triple {
            auto haystack = result.remainder;

            if (haystack.length() < needle.length())
                return !result;

            auto rem = haystack.substr(0, needle.length());

            if (rem != needle)
                return !result;

            auto remainder = haystack.substr(
                needle.length(),
                haystack.length() - needle.length()
            );

            return Triple {
                .success = true,
                .quotient = std::string(result.quotient) + std::string(needle),
                .remainder = remainder,
            };
        }) {}

    Parse(std::function<bool(char)> condition, char c):
        _action([condition, c](Triple result) -> Triple {
            return condition(c)
                ? result.Copy(false)
                : Triple {
                    .success = true,
                    .quotient = std::string(result.quotient) + c,
                    .remainder = result.remainder.substr(
                        1,
                        result.remainder.length() - 1
                    ),
                };
        }) {}

    Parse(char c):
        _action([c](Triple result) -> Triple {
            auto rem = result.remainder;

            return rem.length() == 0 || rem[0] != c
                ? result.Copy(false)
                : Triple {
                    .success = true,
                    .quotient = std::string(result.quotient) + c,
                    .remainder = rem.substr(1, rem.length() - 1),
                };
        }) {}

    Parse(bool decision):
        _action([decision](Triple result) -> Triple {
            return result.Copy(decision);
        }) {}

    virtual functor_t
    Get() const override {
        return _action;
    }
};

const parser_t operator*(const parser_t first, const parser_t secnd) {
    return first->And(secnd);
}

const parser_t operator+(const parser_t first, const parser_t secnd) {
    return first->Or(secnd);
}

const parser_t operator*(const int scalar, const parser_t vector) {
    parser_t temp = Parse::New(true);

    for (int i = 1; i <= scalar; ++i)
        temp = temp->Or(vector);

    return temp;
}

const parser_t Pow(const parser_t vector, const int scalar) {
    parser_t temp = Parse::New(true);

    for (int i = 1; i <= scalar; ++i)
        temp = temp->And(vector);

    return temp;
}

// Seq ::= x Seq | nil
// parse_seq = (x * parse_seq) + parse_true

const parser_t While(const parser_t vector) {
    return Parse::New(functor_t(
        [vector](Triple init) {
            Triple next = vector->Get()(init);

            parser_t nextParse = next.success
                ? While(vector)
                : Parse::New(true)
                ;

            return nextParse->Get()(next);
        }
    ));
}

class ParseEmpty: public AParser {
public:
    virtual ~ParseEmpty() = default;

    static parser_t New() { return ::ptr<ParseEmpty>(); }

    virtual functor_t
    Get() const override {
        return [](Triple result) -> Triple {
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

        auto parse_w = Parse::New('w');
        auto parse_h = Parse::New('h');
        auto parse_a = Parse::New('a');
        auto parse_t = Parse::New('t');
        auto parse_empty = ParseEmpty::New();

        auto parse_what =
            parse_w->And(parse_h)->And(parse_a)->And(parse_t)->And(parse_empty);

        auto final = parse_what->Get()(res);

        if (final.success)
            std::cout
                << "Quotient  : " << final.quotient << '\n'
                << "Remainder : " << final.remainder << '\n'
                ;

        auto parse_hello =
            Parse::New('h') * Parse::New('e') * (Pow(Parse::New('l'), 2)) * Parse::New('o')
            ;

        auto parse_world =
            Parse::New(std::string("world"))
            ;

        final = (
              parse_hello
            * Parse::New(' ')
            * parse_world
            * ParseEmpty::New()
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

        auto parse_while_x = While(Parse::New('x'));

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
