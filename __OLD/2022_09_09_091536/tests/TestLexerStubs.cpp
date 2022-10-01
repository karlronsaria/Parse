#include "TestLexer.h"

void Tests::Init() {
    if (!WorkingDirectory.compare(""))
        std::cout
            << "Tests: WARNING: The working directory has not been set.\n";

    _list = {
        {
            "StreamEnumerator_Should_EnumerateStringStream",
            [](std::string & actual, std::string & expected) -> bool {
                std::string inString;
                std::string expectedString;

                inString = "It's all I have to bring today";
                expected =
                    "[I]\n[t]\n[']\n[s]\n[ ]\n[a]\n[l]\n[l]\n[ ]\n[I]\n[ ]\n[h]\n[a]\n[v]\n[e]\n[ ]\n[t]\n[o]\n[ ]\n[b]\n[r]\n[i]\n[n]\n[g]\n[ ]\n[t]\n[o]\n[d]\n[a]\n[y]\n"
                    ;

                std::stringstream ss(inString);
                std::ostringstream testOut;

                std::unique_ptr<IEnumerator> myEnumerator
                    = std::make_unique<StreamEnumerator>(ss);

                ToStream(*myEnumerator, testOut);
                actual = testOut.str();
                return !expected.compare(actual);
            }
        },
        {
            "Lexer_Should_LexBasicLineOfCpp_WithMinimalWhiteSpace",
            [](std::string & actual, std::string & expected) -> bool {
                std::string testString =
                    "for(int _index1=0;_index2<=12;++_index3){";
                std::vector<std::string> expectedTokens = {
                    "Word: [for]",
                    "Punctuation: [(]",
                    "Word: [int]",
                    "WhiteSpace: [ ]",
                    "Word: [_index1]",
                    "Punctuation: [=]",
                    "Number: [0]",
                    "Punctuation: [;]",
                    "Word: [_index2]",
                    "Punctuation: [<]",
                    "Punctuation: [=]",
                    "Number: [12]",
                    "Punctuation: [;]",
                    "Punctuation: [+]",
                    "Punctuation: [+]",
                    "Word: [_index3]",
                    "Punctuation: [)]",
                    "Punctuation: [{]",
                };

                return TestLexer(
                    testString,
                    std::move(expectedTokens),
                    actual,
                    expected,
                    false
                );
            }
        },
        {
            "Lexer_Should_LexBasicLineOfCpp_IgnoringWhiteSpace",
            [](std::string & actual, std::string & expected) -> bool {
                std::string testString =
                    "for (int _index1 = 0; _index2 <= 12; ++_index3) {";
                std::vector<std::string> expectedTokens = {
                    "Word: [for]",
                    "Punctuation: [(]",
                    "Word: [int]",
                    "Word: [_index1]",
                    "Punctuation: [=]",
                    "Number: [0]",
                    "Punctuation: [;]",
                    "Word: [_index2]",
                    "Punctuation: [<]",
                    "Punctuation: [=]",
                    "Number: [12]",
                    "Punctuation: [;]",
                    "Punctuation: [+]",
                    "Punctuation: [+]",
                    "Word: [_index3]",
                    "Punctuation: [)]",
                    "Punctuation: [{]",
                };

                return TestLexer(
                    testString,
                    std::move(expectedTokens),
                    actual,
                    expected,
                    true
                );
            }
        },
        {
            "Lexer_Should_LexBasicLineOfCpp",
            [](std::string & actual, std::string & expected) -> bool {
                std::string testString =
                    "for (int \t_index1 = 0; \n _index2 <= 12; \n\t ++_index3) {";
                std::vector<std::string> expectedTokens = {
                    "Word: [for]",
                    "WhiteSpace: [ ]",
                    "Punctuation: [(]",
                    "Word: [int]",
                    "WhiteSpace: [ \\t]",
                    "Word: [_index1]",
                    "WhiteSpace: [ ]",
                    "Punctuation: [=]",
                    "WhiteSpace: [ ]",
                    "Number: [0]",
                    "Punctuation: [;]",
                    "WhiteSpace: [ \\n ]",
                    "Word: [_index2]",
                    "WhiteSpace: [ ]",
                    "Punctuation: [<]",
                    "Punctuation: [=]",
                    "WhiteSpace: [ ]",
                    "Number: [12]",
                    "Punctuation: [;]",
                    "WhiteSpace: [ \\n\\t ]",
                    "Punctuation: [+]",
                    "Punctuation: [+]",
                    "Word: [_index3]",
                    "Punctuation: [)]",
                    "WhiteSpace: [ ]",
                    "Punctuation: [{]",
                };

                return TestLexer(
                    testString,
                    std::move(expectedTokens),
                    actual,
                    expected,
                    false
                );
            }
        },
        {
            "Lexer_Should_LexStringAssignment",
            [](std::string & actual, std::string & expected) -> bool {
                std::string testString =
                    "__dickinson := \"It's all I have to bring today\";";
                std::vector<std::string> expectedTokens = {
                    "Word: [__dickinson]",
                    "WhiteSpace: [ ]",
                    "Punctuation: [:]",
                    "Punctuation: [=]",
                    "WhiteSpace: [ ]",
                    "String: [It's all I have to bring today]",
                    "Punctuation: [;]",
                };

                return TestLexer(
                    testString,
                    std::move(expectedTokens),
                    actual,
                    expected,
                    false
                );
            }
        },
        {
            "Lexer_Should_LexCCode",
            [](std::string & actual, std::string & expected) -> bool {
                std::string testString =
                    "while (int _number3 = -12.37 && my3_str__4 == \"Everyone has UNO,\\nit came free with your\\tX-box.\");;";
                std::vector<std::string> expectedTokens = {
                    "Word: [while]",
                    "Punctuation: [(]",
                    "Word: [int]",
                    "Word: [_number3]",
                    "Punctuation: [=]",
                    "Punctuation: [-]",
                    "Float: [12.37]",
                    "Punctuation: [&]",
                    "Punctuation: [&]",
                    "Word: [my3_str__4]",
                    "Punctuation: [=]",
                    "Punctuation: [=]",
                    "String: [Everyone has UNO,\nit came free with your\tX-box.]",
                    "Punctuation: [)]",
                    "Punctuation: [;]",
                    "Punctuation: [;]"
                };

                return TestLexerDemo(
                    testString,
                    std::move(expectedTokens),
                    actual,
                    expected
                );
            }
        },
        {
            "JsonResultSet_Should_MatchStringInNestedObject",
            [](std::string & actual, std::string & expected) -> bool {
                FileReader inputReader;

                if (!StartFileReader(
                    "res/input01.json",
                    actual,
                    inputReader
                )) {
                    expected = "Input file opened successfully";
                    return false;
                }

                auto machine = Json::GetMachine(inputReader.Stream());

                actual =
                    machine->GetResultSet()
                        .At("link")
                        .At("url")
                        .ToString()
                        ;
                expected =
                    "https://json.org/example.html";

                if (expected.compare(actual))
                    return false;

                actual =
                    machine->GetResultSet()
                        .ToString()
                        ;
                expected =
                    "{ \"link\": { \"url\": \"https://json.org/example.html\", \"retrieved\": \"2022_06_19\" }, \"glossary\": { \"title\": \"example glossary\", \"GlossDiv\": { \"title\": \"S\", \"GlossList\": { \"GlossEntry\": { \"ID\": \"SGML\", \"SortAs\": \"SGML\", \"GlossTerm\": \"Standard Generalized Markup Language\", \"Acronym\": \"SGML\", \"Abbrev\": \"ISO 8879:1986\", \"GlossDef\": { \"para\": \"A meta-markup language, used to create markup languages such as DocBook.\", \"GlossSeeAlso\": [ \"GML\", \"XML\" ] }, \"GlossSee\": \"markup\" } } } } }";

                if (expected.compare(actual))
                    return false;

                actual =
                    machine->GetResultSet()
                        .At("glossary")
                        .At("GlossDiv")
                        .At("GlossList")
                        .At("GlossEntry")
                        .At("GlossDef")
                        .At("GlossSeeAlso")
                        .At(1)
                        .ToString()
                        ;
                expected =
                    "XML";

                if (expected.compare(actual))
                    return false;

                actual =
                    machine->GetResultSet()
                        ["glossary"]
                        ["GlossDiv"]
                        ["GlossList"]
                        ["GlossEntry"]
                        ["GlossDef"]
                        ["GlossSeeAlso"]
                        [1]
                        .ToString()
                        ;
                expected =
                    "XML";

                if (expected.compare(actual))
                    return false;

                actual =
                    machine->GetResultSet()
                        .At("glo")
                        .At("Glo")
                        .At("Glo")
                        .At("Glo")
                        .At("Glo")
                        .At("Glo")
                        .At(1)
                        .ToString()
                        ;
                expected =
                    "";

                if (expected.compare(actual))
                    return false;

                actual =
                    machine->GetResultSet()
                        ["glo"]
                        ["Glo"]
                        ["Glo"]
                        ["Glo"]
                        ["Glo"]
                        ["Glo"]
                        [1]
                        .ToString()
                        ;
                expected =
                    "";

                return !expected.compare(actual);
            }
        },
        {
            "VectorBitset_Should_PushBackAndSet",
            [](std::string & actual, std::string & expected) -> bool {
                vector_bitset<> myBitset;

                for (int i = 1; i <= 67; ++i)
                    myBitset.push_back(false);

                myBitset.set(7);
                myBitset.set(53);

                expected = "0000000000000100000000000000000000000000000000000000000000010000000";
                actual = myBitset.to_string();

                if (expected.compare(actual))
                    return false;

                expected = "010";
                std::ostringstream oss;

                oss << myBitset[6]
                    << myBitset[7]
                    << myBitset[8];

                actual = oss.str();

                if (expected.compare(actual))
                    return false;

                oss.str("");

                oss << myBitset[52]
                    << myBitset[53]
                    << myBitset[54];

                actual = oss.str();
                return !expected.compare(actual);
            }
        },
        {
            "JsonBuilder_Should_MatchJsonFileContent",
            [](std::string & actual, std::string & expected) -> bool {
                FileReader expectedReader;

                if (!StartFileReader(
                    "res/expected100.txt",
                    actual,
                    expectedReader
                )) {
                    expected = "Expected file opened successfully";
                    return false;
                }

                Json::Builder builder;

                builder
                    .Object()
                        .Pair("author")
                            .String("karlr")
                        .Pair("date")
                            .String("2022_07_13")
                        .Pair("projects")
                            .List()
                                .Value()
                                    .String("FileReader")
                                .Value()
                                    .String("Lexer")
                                .Value()
                                    .String("JsonBuilder")
                        .Pair("numbers")
                            .List()
                                .Value()
                                    .Integer(137)
                                .Value()
                                    .Float(12.33333337)
                                .Value()
                                    .Boolean(true)
                        .Pair("link")
                            .Object()
                                .Pair("site")
                                    .String("YouTube.com")
                                .Pair("video")
                                    .Object()
                                        .Pair("title")
                                            .String("Unterpants - Genocide Ending (SPOILERS)")
                                        .Pair("author")
                                            .String("Sr Pelo")
                                    .Pop()
                                .Pair("url")
                                    .String("https://www.youtube.com/watch?v=Yw0OeCMB1MM")
                                .Pair("retrieved")
                                    .String("2022_07_13")
                            .Pop()
                        .Pair("what")
                            .String("the")
                    ;

                std::stringstream actualStream;

                actualStream
                    << "MACHINE TO_STRING\n"
                    << "-----------------\n"
                    << builder
                        .Machine()
                        ->ToString()
                    << '\n'
                    << "RESULT_SET TO_STRING\n"
                    << "--------------------\n"
                    << builder
                        .Machine()
                        ->GetResultSet()
                        .ToString()
                    << "\n"
                    ;

                if (0 <= GetNextDifferentLine(
                    expectedReader.Stream(),
                    actualStream,
                    expected,
                    actual
                ))
                    return false;

                expected = "Lexer";
                actual = builder
                    .Machine()
                    ->GetResultSet()
                    .At("projects")
                    .At(1)
                    .ToString()
                    ;

                if (expected.compare(actual))
                    return false;

                expected = "JsonBuilder";
                actual = builder
                    .Machine()
                    ->GetResultSet()
                    ["projects"]
                    [2]
                    .ToString()
                    ;

                return !expected.compare(actual);
            }
        },
        {
            "JsonMachineToString_Should_MatchJsonFileContent",
            [](std::string & actual, std::string & expected) -> bool {
                bool success = TestJsonParserDemo(
                    "res/input01.json",
                    "res/expected011.txt",
                    actual,
                    expected,
                    [](const auto & machine) { return machine.ToString(); }
                );

                return success;
            }
        },
        {
            "JsonParserErrorMessage_Should_IdentifyMisplacedPunctuation",
            [](std::string & actual, std::string & expected) -> bool {
                bool success = TestJsonParserDemo(
                    "res/input03_incorrect.txt",
                    "res/expected030.txt",
                    actual,
                    expected
                );

                return success;
            }
        },
        {
            "JsonParserErrorMessage_Should_IdentifyPrematureEof",
            [](std::string & actual, std::string & expected) -> bool {
                bool success = TestJsonParserDemo(
                    "res/input04_incorrect.txt",
                    "res/expected040.txt",
                    actual,
                    expected
                );

                return success;
            }
        }
    };
}
