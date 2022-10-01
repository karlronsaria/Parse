#include "MyJson.h"

Json::MyLexer::MyLexer(
    std::shared_ptr<IEnumerator> && stream
):  Json::Lexer(std::move(stream)) {}

int Json::MyLexer::NextToken() {
    std::ostringstream oss;
    int code = Lexer::NextToken();
    std::string token;

    switch ((Token)code) {
        case Token::STRING:
            token = "\"" + String() + '"';
            break;
        case Token::WORD:
            token = String();
            break;
        case Token::INTEGER:
            oss << Integer();
            token = oss.str();
            break;
        case Token::FLOAT:
            oss << Float();
            token = oss.str();
            break;
        case Token::END:
        case Token::ERROR:
            token = "";
            break;
        default:
            token = std::string(1, (char)code);
            break;
    }

    _codes.push_back(code);
    _tokens.push_back(token);
    return code;
}

const std::vector<int> &
Json::MyLexer::Codes() const {
    return _codes;
}

const std::vector<std::string> &
Json::MyLexer::Tokens() const {
    return _tokens;
}

#ifdef JSON_TREE_PARAMETER
#undef JSON_TREE_PARAMETER
#endif
#define JSON_TREE_PARAMETER Json::Pointer

#ifdef JSON_FACTORY_PTR 
#undef JSON_FACTORY_PTR
#endif
#define JSON_FACTORY_PTR Json::ITreeFactory<Json::Tree<JSON_TREE_PARAMETER>>::ptr_t

JSON_FACTORY_PTR
Json::MyTreeFactory::NewObject(
    std::vector<std::string> keys,
    std::vector<ptr_t> values
) {
    return std::make_unique<Object<JSON_TREE_PARAMETER>>(
        std::move(keys),
        std::move(values)
    );
}

JSON_FACTORY_PTR
Json::MyTreeFactory::NewList(
    std::vector<ptr_t> values
) {
    return std::make_unique<List<JSON_TREE_PARAMETER>>(
        std::move(values)
    );
}

JSON_FACTORY_PTR
Json::MyTreeFactory::NewString(
    const std::string & value
) {
    return std::make_unique<String<JSON_TREE_PARAMETER>>(value);
}

JSON_FACTORY_PTR
Json::MyTreeFactory::NewNumeric(
    Homonumeric value
) {
    return std::make_unique<Numeric<JSON_TREE_PARAMETER>>(value);
}

JSON_TREE_PARAMETER
Json::MyPostorderTreeVisitor::ForObject(
    const std::vector<std::string> & keys,
    std::vector<JSON_TREE_PARAMETER> && values
) {
    auto objectPtr = _machine->NewObject();
    auto & object = _machine->Object(objectPtr.key);

    for (int i = 0; i < keys.size(); ++i)
        // TODO: Add collision checking
        object.add(keys[i], values[i]);

    return objectPtr;
}

JSON_TREE_PARAMETER
Json::MyPostorderTreeVisitor::ForList(
    std::vector<JSON_TREE_PARAMETER> && values
) {
    auto listPtr = _machine->NewList();
    auto & list = _machine->List(listPtr.key);

    for (auto & value : values)
        list.push_back(value);

    return listPtr;
}

JSON_TREE_PARAMETER
Json::MyPostorderTreeVisitor::ForNumeric(
    Homonumeric value
) {
    switch (value.Mode) {
        case Homonumeric::Mode::BOOLEAN:
            return _machine->NewBoolean(value.Payload.Boolean);
        case Homonumeric::Mode::INTEGER:
            return _machine->NewInteger(value.Payload.Integer);
        case Homonumeric::Mode::FLOAT:
            return _machine->NewFloat(value.Payload.Float);
        default:
            // TODO: Consider adding exception-handling here
            break;
    }

    return JSON_TREE_PARAMETER();
}

JSON_TREE_PARAMETER
Json::MyPostorderTreeVisitor::ForString(
    const std::string & value
) {
    return _machine->NewString(value);
}

std::shared_ptr<const Json::Machine>
Json::GetMachine(
    std::istream & inputStream
) {
    return GetMutableMachine(inputStream);
}

std::shared_ptr<Json::Machine>
Json::GetMutableMachine(std::istream & inputStream) {
    auto enumerator = std::make_shared<StreamEnumerator>(inputStream);
    // auto lexer = std::make_shared<Lexer>(enumerator);
    // auto lexer = std::make_shared<Json::Lexer>(enumerator);
    auto lexer = std::make_shared<Json::MyLexer>(enumerator);
    auto machine = std::make_shared<Json::Machine>();
    auto visitor = std::make_shared<Json::MyPostorderTreeVisitor>(machine);
    auto parse = Json::Parser<Json::Tree<Json::Pointer>>::Tree<Json::MyTreeFactory>(lexer);
    // auto parse = Json::Parser<Json::Tree<Json::Pointer>>::Tree<Json::MyTreeFactory>();

    if (parse.Success)
        parse.Tree->Accept(visitor);

    return machine;
}

void
Json::ParserMessageToStream(
    std::ostream & outStream,
    const MyLexer & lexer,
    const std::string & message
) {
    const int MAX_COL2 = 40;
    const int MIN_COL1 = 4;
    std::ostringstream stream;

    for (const std::string & token : lexer.Tokens())
        stream << token << ' ';

    std::string lastToken = lexer.Tokens().back();
    std::string history = stream.str();

    stream.str("");

    std::string underline;
    int underlineLength = MAX_COL2;
    int trailingSpaces = 0;

    switch (lexer.Codes().back()) {
        case Token::END:
        case Token::ERROR:
            underlineLength = MAX_COL2 + 2;
            underline = "~";
            trailingSpaces = 2;
            break;
        default:
            stream
                << std::setfill('~')
                << std::setw(lastToken.length())
                << '~'
                ;

            trailingSpaces = 1;
            underline = stream.str();
            stream.str("");
            break;
    }

    history = history.substr(history.length() - MAX_COL2 - trailingSpaces, MAX_COL2);
    stream << lexer.Index();
    std::string index = stream.str();

    int margin = index.length() + 1 > MIN_COL1
        ? index.length() + 1
        : MIN_COL1
        ;

    outStream
        << "Json::Parser:\n"
        << std::setw(margin) << "At" << " |\n"
        << std::setw(margin) << lexer.Index() << " |  "
            << history << '\n'
        << std::setw(margin) << ' ' << " |  "
            << std::setw(underlineLength) << underline << '\n'
        << std::setw(margin) << ' ' << " | "
            << message << '\n'
        ;
}

Json::MyResultSet
Json::RunMyParser(
    std::istream & inputStream
) {
    auto enumerator = std::make_shared<StreamEnumerator>(inputStream);
    auto lexer = std::make_shared<Json::MyLexer>(enumerator);
    auto machine = std::make_shared<Json::Machine>();
    auto visitor = std::make_shared<Json::MyPostorderTreeVisitor>(machine);
    auto parse = Json::Parser<Json::Tree<Json::Pointer>>::Tree<Json::MyTreeFactory>(lexer);

    MyResultSet result;

    result.Success = parse.Success;
    result.Message = "";
    result.Machine = nullptr;

    if (!parse.Success) {
        result.Message
            = ParserMessageToString(*lexer, parse.Message);

        return result;
    }

    parse.Tree->Accept(visitor);
    result.Machine = machine;
    return result;
}

std::string
Json::ParserMessageToString(
    const MyLexer & lexer,
    const std::string & message
) {
    std::ostringstream out;
    ParserMessageToStream(out, lexer, message);
    return out.str();
}

#undef JSON_FACTORY_PTR
#undef JSON_TREE_PARAMETER