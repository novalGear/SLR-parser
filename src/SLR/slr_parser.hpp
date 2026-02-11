#pragma once

class SLR_parser {
public:
    // Конструктор: принимает входной поток (например, std::cin или ifstream)
    explicit SLRParser(std::istream& input);

    // Запуск разбора
    void parse();

private:
    std::istream& input_stream_;            // Входной поток
    Token current_token_;                   // Текущий токен (после вызова next_token())
    std::vector<int> state_stack_;          // Стек состояний (для SLR — стек номеров состояний DFA)
    std::vector<Token> symbol_stack_;       // Стек символов (нетерминалы/терминалы — опционально, если строите AST)

    // Чтение следующего токена из input_stream_
    void next_token();

    // Операции автомата
    void shift(int next_state);
    void reduce(int rule_index);
    void accept();
    void error(const char* message);
    void warning(const char* message);

    // Вспомогательные методы
    int current_state() const;  // вершина state_stack_
    bool is_eof() const;
};
}
