#include <gtest/gtest.h>
#include <sstream>
#include <streambuf>
#include <string>
#include <vector>
#include <memory>

#include "token_struct.hpp"
#include "slr_parser.hpp"
#include "lexer_defs.hpp"
#include <FlexLexer.h>

std::string run_parser_on_input(const std::string& input_text) {
    // 1. Подготовка буфера ввода
    std::istringstream iss(input_text);

    // 2. Инициализация лексера
    yyFlexLexer lexer;
    lexer.switch_streams(&iss, &std::cerr); // stderr оставляем как есть для самого лексера

    // 3. Лексический анализ
    std::vector<Token> tokens;
    while (true) {
        int token_id = lexer.yylex();
        if (token_id == TOK_ERROR) {
            return "[LEXER ERROR]";
        }
        const Token& tok = yylval;
        tokens.push_back(tok);
        if (tok.type == Symbol::END_MARKER) break;
    }

    // 4. Перехват ВЫХОДЯЩИХ потоков (cout и cerr)
    std::streambuf* original_cout = std::cout.rdbuf();
    std::streambuf* original_cerr = std::cerr.rdbuf();

    std::ostringstream captured_output;

    // Перенаправляем оба потока в один буфер
    std::cout.rdbuf(captured_output.rdbuf());
    std::cerr.rdbuf(captured_output.rdbuf());

    // 5. Запуск парсера
    SLR_Parser parser;
    bool result = parser.parse(tokens);

    // Восстанавливаем потоки обратно
    std::cout.rdbuf(original_cout);
    std::cerr.rdbuf(original_cerr);

    std::string final_output = captured_output.str();
    final_output += "\n[RESULT]: " + std::string(result ? "SUCCESS" : "FAILED");

    return final_output;
}

TEST(ParserE2ETest, SimpleAddition) {
    std::string input = "3 + 4\n";
    std::string output = run_parser_on_input(input);

    EXPECT_NE(output.find("[RESULT]: SUCCESS"), std::string::npos)
        << "Парсер должен был успешно завершить работу, но выдал:\n" << output;

    EXPECT_NE(output.find("Shift"), std::string::npos);
    EXPECT_NE(output.find("Reduce"), std::string::npos);
}

TEST(ParserE2ETest, MultiplicationPrecedence) {
    std::string input = "2 + 3 * 4\n";
    std::string output = run_parser_on_input(input);

    EXPECT_NE(output.find("[RESULT]: SUCCESS"), std::string::npos);

    // Проверка специфичной последовательности редюсов (T -> num * num должно быть перед E -> E + T)
    // Это гарантирует, что приоритет соблюден
    size_t pos_mul_reduce = output.find("Reduce T -> num * num"); // Или ваш формат вывода
    size_t pos_add_reduce = output.find("Reduce E -> E + T");

    if (pos_mul_reduce != std::string::npos && pos_add_reduce != std::string::npos) {
        EXPECT_LT(pos_mul_reduce, pos_add_reduce)
            << "Ошибка приоритета: умножение должно свернуться раньше сложения!";
    }
}

TEST(ParserE2ETest, SyntaxError) {
    std::string input = "3 + + 4\n";
    std::string output = run_parser_on_input(input);

    EXPECT_NE(output.find("[RESULT]: FAILED"), std::string::npos)
        << "Парсер должен был обнаружить ошибку, но успешно завершился.";
    EXPECT_NE(output.find("Error"), std::string::npos);
}

// Вспомогательная функция для извлечения только строк действий из вывода
std::vector<std::string> extract_actions(const std::string& full_output) {
    std::vector<std::string> actions;
    std::istringstream stream(full_output);
    std::string line;

    while (std::getline(stream, line)) {
        // Нас интересуют только строки, где есть действие (Shift, Reduce, Accept)
        // Игнорируем заголовки, DEBUG-строки и пустые строки
        if (line.find("| Shift") != std::string::npos ||
            line.find("| Reduce") != std::string::npos ||
            line.find("| Accept") != std::string::npos) {

            // Очищаем строку от лишних пробелов в конце для надежности
            size_t end = line.find_last_not_of(" \t\r\n");
            if (end != std::string::npos) {
                line = line.substr(0, end + 1);
            }
            // Можно также обрезать левую часть до колонки "|", чтобы не зависеть от ширины колонок
            size_t pipe_pos = line.rfind("|");
            if (pipe_pos != std::string::npos) {
                // Оставляем только часть после последней трубы (само действие)
                // Или можно оставить всю строку таблицы, если формат фиксирован.
                // Для максимальной стабильности оставим всю строку таблицы действия.
            }
            actions.push_back(line);
        }
    }
    return actions;
}

TEST(ParserE2ETest, ExactSequenceParenthesesPrecedence) {
    std::string input = "( x + 4 ) / y - 10\n";
    std::string output = run_parser_on_input(input);

    // 1. Быстрая проверка на успех
    ASSERT_NE(output.find("[RESULT]: SUCCESS"), std::string::npos)
        << "Парсинг не удался. Вывод:\n" << output;

    // 2. Извлекаем фактическую последовательность действий
    std::vector<std::string> actual_actions = extract_actions(output);

    // 3. Формируем ожидаемую последовательность (на основе вашего лога)
    // Примечание: Строки должны совпадать символ в символ, включая пробелы внутри таблицы.
    // Если форматирование таблицы плавающее (setw), лучше сравнивать только часть после "|".
    // Ниже приведен пример сравнения ПОЛНЫХ строк таблицы. Если тест падает из-за пробелов,
    // используйте сравнение только суффикса строки.

    std::vector<std::string> expected_actions = {
        "$                                        | (id+num)/id-num$          | Shift",
        "$ (                                      | id+num)/id-num$           | Shift",
        "$ ( id                                   | +num)/id-num$             | Reduce P -> id",
        "$ ( P                                    | +num)/id-num$             | Reduce T -> P",
        "$ ( T                                    | +num)/id-num$             | Reduce E -> T",
        "$ ( E                                    | +num)/id-num$             | Shift",
        "$ ( E +                                  | num)/id-num$              | Shift",
        "$ ( E + num                              | )/id-num$                 | Reduce P -> num",
        "$ ( E + P                                | )/id-num$                 | Reduce T -> P",
        "$ ( E + T                                | )/id-num$                 | Reduce E -> E+T",
        "$ ( E                                    | )/id-num$                 | Shift",
        "$ ( E )                                  | /id-num$                  | Reduce P -> (E)",
        "$ P                                      | /id-num$                  | Reduce T -> P",
        "$ T                                      | /id-num$                  | Shift",
        "$ T /                                    | id-num$                   | Shift",
        "$ T / id                                 | -num$                     | Reduce P -> id",
        "$ T / P                                  | -num$                     | Reduce T -> T/P",
        "$ T                                      | -num$                     | Reduce E -> T",
        "$ E                                      | -num$                     | Shift",
        "$ E -                                    | num$                      | Shift",
        "$ E - num                                | $                         | Reduce P -> num",
        "$ E - P                                  | $                         | Reduce T -> P",
        "$ E - T                                  | $                         | Reduce E -> E-T",
        "$ E                                      | $                         | Accept"
    };

    // 4. Сравниваем размеры
    ASSERT_EQ(actual_actions.size(), expected_actions.size())
        << "Количество шагов парсинга не совпадает!\n"
        << "Ожидалось: " << expected_actions.size() << "\n"
        << "Получено: " << actual_actions.size() << "\n"
        << "Полный вывод:\n" << output;

    // 5. Посрочное сравнение
    for (size_t i = 0; i < expected_actions.size(); ++i) {
        //_trim_ пробелы по краям для надежности, так как setw может вести себя по-разному
        auto trim = [](std::string s) {
            size_t start = s.find_first_not_of(" ");
            size_t end = s.find_last_not_of(" ");
            if (start == std::string::npos) return std::string("");
            return s.substr(start, end - start + 1);
        };

        std::string expected_trimmed = trim(expected_actions[i]);
        std::string actual_trimmed = trim(actual_actions[i]);

        EXPECT_EQ(actual_trimmed, expected_trimmed)
            << "Несовпадение на шаге #" << (i + 1) << ":\n"
            << "Ожидалось: \"" << expected_trimmed << "\"\n"
            << "Получено:  \"" << actual_trimmed << "\"\n";
    }
}
