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

        auto parse_while_x = While(Parse<Triple, Triple>::Char('x'));
        PrintTest(parse_while_x, "xxxxxxxaaaa");

        auto integer = Parsers::Integer<Integral>();
        PrintTest(integer, "19991xxxx");
        PrintTest(integer, "09991xxxx");
        PrintTest(integer, "00991xxxx");
        PrintTest(integer, "10000xxxx");

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

        Integral i1 = Integral::New(what, 0);
        Integral i2 = integer->Fn()(i1);

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
                    Integral i2 = Parsers::Integer<Integral>()->Fn()(
                        Integral::New(i.Triple(), 0)
                    );

                    float f1 = Converts::ToFloat(i.value, i2.value);
                    return Dividend<float>::New(i2.Triple(), f1);
                }
            );

        if (i2.Success()) {
            /*
            Triple t1 = i2.Triple();
            int v1 = i2.value;

            Triple t2 = Parse<Triple, Triple>::Char('.')->Fn()(t1);
            */

            // Integral i3 = has_point->Fn()(i2);

            Dividend<float> f1 = (has_point * has_tail)->Fn()(i2);
            std::cout << f1.value << '\n';

            /*
            if (i3.Success()) {
                // Integral i3 = Integral::New(t2, 0);
                Integral i4 = integer->Fn()(Integral::New(i3.Triple(), 0));

                float f1 = Converts::ToFloat(i3.value, i4.value);

                std::cout << f1 << '\n';

                // Dividend<float>::New(i4.Triple(), Converts::ToFloat(v1, i4.value));
            }
            */
        }
    }
    #endif

    return 0;
}
