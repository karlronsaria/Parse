#include "TestLexer.h"

LexerTestVisitor::LexerTestVisitor(
    const std::shared_ptr<Lexer> & lexer,
    const std::vector<std::string> & expectedTokens,
    bool ignoreWhiteSpace
):  _lexer(lexer),
    _expected_tokens(expectedTokens),
    _ignore_white_space(ignoreWhiteSpace),
    _index(0) {
}

void LexerTestVisitor::ForWhiteSpace(const std::string & whiteSpace) {
    if (!_ignore_white_space)
        _test_out
            << "WhiteSpace: ["
            << ToReadableWhiteSpace(whiteSpace)
            << "]";
}

void LexerTestVisitor::ForWord(const std::string & word) {
    _test_out << "Word: [" << word << "]";
}

void LexerTestVisitor::ForInteger(int number) {
    _test_out << "Number: [" << number << "]";
}

int LexerTestVisitor::ForPunctuation(char token) {
    switch (token) {
        case STRING_DELIMITER:
            token = (char)_lexer->LexString(STRING_DELIMITER);

            switch (token) {
                case Token::END:
                    break;
                case Token::STRING:
                    _test_out << "String: [" << _lexer->String() << "]";
                    break;
                default:
                    // TODO: Error
                    break;
            }

            break;
        default:
            _test_out << "Punctuation: [" << token << "]";
            break;
    }

    return (int)token;
}

bool LexerTestVisitor::ForEachToken(int token) {
    if (_index >= ExpectedMax()) {
        _actual = ToString(_index);
        _expected = ToString(ExpectedMax() - 1);
        return false;
    }

    if (!_ignore_white_space || token != Token::SPACE) {
        _actual = _test_out.str();
        _expected = _expected_tokens[_index];

        if (_expected.compare(_actual))
            return false;

        _test_out.str("");
        _index = _index + 1;
    }

    return true;
}

std::shared_ptr<Lexer> LexerTestVisitor::GetLexer() {
    return _lexer;
}

const std::string & LexerTestVisitor::Actual() const {
    return _actual;
}

const std::string & LexerTestVisitor::Expected() const {
    return _expected;
}

int LexerTestVisitor::Index() const {
    return _index;
}

int LexerTestVisitor::ExpectedMax() const {
    return _expected_tokens.size();
}

std::vector<Test> Tests::_list;
std::string Tests::WorkingDirectory;

void Tests::Run(std::ostream & out) {
    std::string actual;
    std::string expected;

    for (auto test : _list) {
        if (test.definition(actual, expected)) {
            out << test.name << ":\n✅ Success\n\n";
        }
        else {
            out << test.name << ":\n❌ Failure\n";
            out << "\n  Actual   : [" << actual << "]";
            out << "\n  Expected : [" << expected << "]\n";
        }
    }
}

bool Tests::TestLexerDemo(
    const std::string & testString,
    const std::vector<std::string> & expectedTokens,
    std::string & actual,
    std::string & expected
) {
    std::ostringstream oss;
    std::stringstream ss(testString);

    Lexer myLexer(
        std::make_shared<StreamEnumerator>(ss)
    );

    bool hasNext = true;
    int index = 0;
    int token;

    while (hasNext) {
        token = Json::Parsing::NextToken(myLexer);

        switch(token) {
            case Token::ERROR:
                oss << "Error at index: "
                    << myLexer.Index();
            case Token::END:
                hasNext = false;
                continue;
            case Token::WORD:
                oss << "Word: ["
                    << myLexer.String()
                    << ']';
                break;
            case Token::CHARACTER:
                oss << "Character: ["
                    << myLexer.Character()
                    << ']';
                break;
            case Token::INTEGER:
                oss << "Integer: ["
                    << myLexer.Integer()
                    << ']';
                break;
            case Token::FLOAT:
                oss << "Float: ["
                    << myLexer.Float()
                    << ']';
                break;
            case Token::STRING:
                oss << "String: ["
                    << myLexer.String()
                    << ']';
                break;
            default:
                oss << "Punctuation: ["
                    << (char)token
                    << ']';
                break;
        }

        expected = expectedTokens[index];
        actual = oss.str();
        oss.str("");

        if (expected.compare(actual))
            return false;

        index = index + 1;
    }

    actual = ToString(index);
    expected = ToString(expectedTokens.size());
    return !expected.compare(actual);
}

bool Tests::TestLexer(
    const std::string & testString,
    const std::vector<std::string> & expectedTokens,
    std::string & actual,
    std::string & expected,
    bool ignoreWhiteSpace = true
) {
    std::stringstream ss(testString);

    auto myLexer = std::make_shared<Lexer>(
        std::make_shared<StreamEnumerator>(ss)
    );

    auto myVisitor = std::make_shared<LexerTestVisitor>(
        myLexer,
        std::move(expectedTokens),
        ignoreWhiteSpace
    );

    if (ForEachToken(*myLexer, myVisitor)) {
        actual = ToString(myVisitor->Index());
        expected = ToString(myVisitor->ExpectedMax());
    }
    else {
        actual = myVisitor->Actual();
        expected = myVisitor->Expected();
    }

    return !expected.compare(actual);
}

bool Tests::StartFileReader(
    const std::string & testFilePath,
    std::string & message,
    FileReader & inputReader
) {
    std::string testFile
        = Tests::WorkingDirectory
            + std::string("/")
            + testFilePath;

    if (!FileReader::New(testFile, inputReader)) {
        message = "Could not open file at "
            + testFile
            + '\n';

        return false;
    }

    return true;
}

bool Tests::TestJsonParserDemo(
    const std::string & testFilePath,
    const std::string & expectedFilePath,
    std::string & actual,
    std::string & expected,
    std::string (*forMachine)(const Json::Machine &)
) {
    FileReader inputReader;

    if (!StartFileReader(
        testFilePath,
        actual,
        inputReader
    )) {
        expected = "Input file opened successfully";
        return false;
    }

    std::stringstream actualStream;
    std::ifstream expectedStream;

    std::string expectedFile
        = Tests::WorkingDirectory
            + std::string("/")
            + expectedFilePath;

    FileReader expectedReader;

    if (!FileReader::New(expectedFile, expectedReader)) {
        std::ostringstream oss;

        oss << "Could not open file at "
            << expectedFile
            << '\n';

        actual = oss.str();
        expected = "";
        return false;
    }

    auto result = Json::RunMyParser(inputReader.Stream());

    if (result.Success) {
        actualStream
            << forMachine(*result.Machine)
            ;
    }
    else {
        actualStream
            << result.Message
            << '\n'
            ;
    }

    return 0 > GetNextDifferentLine(
        expectedReader.Stream(),
        actualStream,
        expected,
        actual
    );
}

bool Tests::TestJsonParserDemo(
    const std::string & testFilePath,
    const std::string & expectedFilePath,
    std::string & actual,
    std::string & expected
) {
    return TestJsonParserDemo(
        testFilePath,
        expectedFilePath,
        actual,
        expected,
        [](const Json::Machine & machine) {
            return machine
                .GetResultSet()
                .ToString()
                ;
        }
    );
}
