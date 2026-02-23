// src/SLR/main.cpp

#include <iostream>
#include <vector>
#include <string>
#include <FlexLexer.h>      // Для yyFlexLexer

#include "../lexer/lexer_defs.hpp"
#include "../lexer/token_struct.hpp"
#include "slr_parser.hpp"

// Функция для отладочного вывода имени токена (опционально)
std::string symbol_to_string_debug(Symbol s) {
    switch (s) {
        case Symbol::PLUS:       return "PLUS";
        case Symbol::MINUS:      return "MINUS";
        case Symbol::STAR:       return "STAR";
        case Symbol::SLASH:      return "SLASH";
        case Symbol::LPAREN:     return "LPAREN";
        case Symbol::RPAREN:     return "RPAREN";
        case Symbol::NUMBER:     return "NUMBER";
        case Symbol::VAR:        return "VAR";
        case Symbol::END_MARKER: return "EOF";
        default:                 return "UNKNOWN";
    }
}

int main() {
    std::cout << "=== SLR Parser with Lexical Analysis ===\n";
    std::cout << "Enter expression (end with Ctrl+D on Linux/Mac or Ctrl+Z on Windows):\n";

    // 1. Инициализация лексера
    yyFlexLexer lexer;
    lexer.switch_streams(&std::cin, &std::cerr);

    // Контейнер для токенов.
    // Теперь каждый токен внутри уже содержит Symbol в поле .type
    std::vector<Token> tokens;

    // 2. Цикл лексического анализа
    while (true) {
        // Вызываем метод yylex() у объекта лексера
        int token_id = lexer.yylex();

        // Проверка на ошибку лексера
        if (token_id == TOK_ERROR) {
            std::cerr << "\n[Lexer Error] Stopping due to lexical error.\n";
            return 1;
        }

        // Берем текущий токен из глобальной переменной yylval
        const Token& current_token = yylval;

        // Добавляем токен в список
        tokens.push_back(current_token);

        // Отладочный вывод (можно закомментировать)
        /*
        std::cout << "[Lex] Line " << current_token.line
                  << ": " << symbol_to_string_debug(current_token.type)
                  << " (" << current_token.text << ")\n";
        */

        // Если достигнут конец файла — прекращаем чтение
        // Сравниваем напрямую с Symbol::END_MARKER
        if (current_token.type == Symbol::END_MARKER) {
            break;
        }
    }

    std::cout << "\n--- Tokens Read: " << tokens.size() << " ---\n";

    // 3. Инициализация и запуск парсера
    SLR_Parser parser;

    // Передаем вектор токенов напрямую.
    // Функция parse ожидает: bool parse(const std::vector<Token>& tokens)
    parser.init();
    bool result = parser.parse(tokens);

    if (result) {
        std::cout << "\n[Success] Input parsed correctly!\n";
        return 0;
    } else {
        std::cout << "\n[Failure] Parsing failed.\n";
        return 1;
    }
}
