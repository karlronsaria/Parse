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

void PrintTest(std::shared_ptr<AParse<Triple>> p, const std::string & dividend) {
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

void PrintTest(std::shared_ptr<AParse<Integral>> p, const std::string & dividend) {
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

        PrintTest(parse_what, "what");

        auto parse_hello =
            Parse<Triple>::New('h')
          * Parse<Triple>::New('e')
          * (Pow(Parse<Triple>::New('l'), 2))
          * Parse<Triple>::New('o')
            ;

        auto parse_world =
            Parse<Triple>::New(std::string("world"))
            ;

        PrintTest(
            parse_hello
          * Parse<Triple>::New(' ')
          * parse_world
          * ParseEmpty<Triple>::New(),
          "hello world"
        );

        auto parse_while_x = While(Parse<Triple>::New('x'));
        PrintTest(parse_while_x, "xxxxxxxaaaa");

        auto integer = Parsers::Integer<Integral>();
        PrintTest(integer, "19991xxxx");
        PrintTest(integer, "09991xxxx");
        PrintTest(integer, "00991xxxx");
        PrintTest(integer, "10000xxxx");
    }
    #endif

    return 0;
}
