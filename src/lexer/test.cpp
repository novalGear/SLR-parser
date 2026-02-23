#include <iostream>
#include <FlexLexer.h> // Стандартный заголовок Flex для C++
#include "lexer_defs.hpp"
#include "token_type.hpp"

// Функция для вывода типа (как у вас уже есть)
std::string type_to_string(TokenType t) {
    switch (t) {
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::STAR: return "STAR";
        case TokenType::SLASH: return "SLASH";
        case TokenType::LPAREN: return "LPAREN";
        case TokenType::RPAREN: return "RPAREN";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::VAR: return "VAR";
        case TokenType::END_OF_FILE: return "END_OF_FILE";
        case TokenType::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

int main() {
    std::cout << "--- SLR Lexer Test (C++ Mode) ---\n";

    // Создаем объект лексера
    yyFlexLexer lexer;

    // Переключаем входной поток на stdin (по умолчанию он тоже stdin, но явно надежнее)
    lexer.switch_streams(&std::cin, &std::cerr);

    std::cout << "Вводите выражения (Ctrl+D для выхода):\n";

    while (true) {
        // Вызываем метод yylex() у объекта lexer
        int token_id = lexer.yylex();

        if (token_id == TOK_ERROR) {
            std::cerr << "Лексическая ошибка!\n";
            continue;
        }

        // Глобальный yylval должен был заполниться внутри метода yylex()
        const Token& tok = yylval;

        std::cout << "Line: " << tok.line
                  << " | Type: " << type_to_string(tok.type)
                  << " | Text: \"" << tok.text << "\"";

        if (tok.isNumber()) {
            std::cout << " | Value(num): " << tok.getNumber();
        } else if (tok.isIdentifier()) {
            std::cout << " | Value(id): " << tok.getIdentifier();
        }
        std::cout << "\n";

        if (token_id == TOK_END_FILE || tok.type == TokenType::END_OF_FILE) {
            break;
        }
    }

    return 0;
}
