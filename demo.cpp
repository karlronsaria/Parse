#include <iostream>
#include "lib/Parsers.h"

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

void PrintTest(std::shared_ptr<AParse<Triple, Triple>> p, const std::string & dividend) {
    auto final = p->Fn()(
        Triple {
            .quotient = "",
            .remainder = dividend,
        }
    );

    if (!final.success)
        std::cout << "Fail:\n";

    std::cout
        << "Quotient  : " << final.quotient << '\n'
        << "Remainder : " << final.remainder << "\n\n"
        ;
}

void PrintTest(std::shared_ptr<AParse<Integral, Integral>> p, const std::string & dividend) {
    auto final = p->Fn()(Integral::New(
        Triple {
            .success = true,
            .quotient = "",
            .remainder = dividend,
        },
        0
    ));

    if (!final.success)
        std::cout << "Fail:\n";

    std::cout
        << "Quotient  : " << final.quotient << '\n'
        << "Remainder : " << final.remainder << '\n'
        << "Value     : " << final.value << '\n'
        << '\n'
        ;
}

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
        auto parse_w = Parse<Triple, Triple>::Char('w');
        auto parse_h = Parse<Triple, Triple>::Char('h');
        auto parse_a = Parse<Triple, Triple>::Char('a');
        auto parse_t = Parse<Triple, Triple>::Char('t');
        auto parse_empty = ParseEmpty<Triple>::New();

        auto parse_what =
            parse_w
            * parse_h
            * parse_a
            * parse_t
            * parse_empty
            ;

        PrintTest(parse_what, "what");

        auto parse_hello =
            Parse<Triple, Triple>::Char('h')
          * Parse<Triple, Triple>::Char('e')
          * (Pow(Parse<Triple, Triple>::Char('l'), 2))
          * Parse<Triple, Triple>::Char('o')
            ;

        auto parse_world =
            Parse<Triple, Triple>::String(std::string("world"))
            ;

        PrintTest(
            parse_hello
          * Parse<Triple, Triple>::Char(' ')
          * parse_world
          * ParseEmpty<Triple>::New(),
          "hello world"
        );

        auto parse_while_x = Some(Parse<Triple, Triple>::Char('x'));
        PrintTest(parse_while_x, "xxxxxxxaaaa");

        auto natural = Parsers::Natural<Integral>();
        PrintTest(natural, "19991xxxx");
        PrintTest(natural, "09991xxxx");
        PrintTest(natural, "00991xxxx");
        PrintTest(natural, "10000xxxx");

        auto triple_to_integer =
            Parse<Triple, Integral>::Functor(
                [](Triple t) -> Integral {
                    std::cout
                        << "I have received your Triple: ("
                        << t.success
                        << ", \""
                        << t.quotient
                        << "\", \""
                        << t.remainder
                        << "\").\n"
                        << "Here is an Integral.\n"
                        ;

                    return Integral::New(t, 125);
                }
            );

        auto final = triple_to_integer->Fn()(
            Triple {
                .success = true,
                .quotient = "",
                .remainder = "Hello, nothing!",
            }
        );

        if (!final.success)
            std::cout << "Fail:\n";

        std::cout
            << "Quotient  : " << final.quotient << '\n'
            << "Remainder : " << final.remainder << '\n'
            << "Value     : " << final.value << '\n'
            << '\n'
            ;

        auto what = Triple {
            .success = true,
            .quotient = "",
            .remainder = "1.2468",
        };

        std::cout << (natural * Parsers::Float())->Fn()(Integral::New(what, 0)).value << '\n';
    }
    #endif

    return 0;
}
