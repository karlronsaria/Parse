#include <iostream>
#include "lib/Parse.h"

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
