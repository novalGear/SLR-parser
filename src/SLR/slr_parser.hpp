#pragma once

#include "../grammar/item.hpp"
#include "../grammar/first_follow.hpp"
#include "../lexer/token_struct.hpp"
#include <vector>
#include <map>
#include <string>

enum class SLR_Error {
    NONE,
    SHIFT_REDUCE_CONFLICT,
    REDUCE_REDUCE_CONFLICT,
};

enum class ActionType {
    SHIFT,
    REDUCE,
    ACCEPT,
    ERROR
};

struct Action {
    ActionType type;
    // SHIFT: номер состояния, REDUCE: номер грамматического правила
    int value;
};

struct State {
    ItemSet items;                          // набор пунктов состояния
    std::map<Symbol, size_t> transitions;   // X -> next_state_id
};


// [state][terminal]
using ActionTable = std::vector<std::vector<Action>>;

// [state][nonterminal]
using GotoTable = std::vector<std::vector<int>>;

std::string format_action(const Action& act);

class SLR_Parser {
public:
    SLR_Parser();

    bool parse(const std::vector<Token>& tokens);

    // интерфейс парсера
    const Action& get_action(size_t state, Symbol terminal) const;
    int get_goto(size_t state, Symbol nonterminal) const;

    // debug
    void dump_parser_state(
        const std::vector<int>& state_stack,
        const std::vector<Token>& token_stack,  // Стек токенов
        const std::vector<Symbol>& input,       // Полный входной массив
        size_t input_pos,                       // Позиция текущего токена
        const Action& action                    // Текущее действие
    );
    void dump_action_table_csv(const std::string& filename) const;

private:
    ActionTable action_table_;
    GotoTable   goto_table_;

    // строим конечный автомат для построения таблиц
    int build_dfa();
    // строим сами таблицы
    int build_tables();

    // сохраняем в json action и goto таблицы
    void save_to_json(const std::string& filename) const;
    bool load_from_json(const std::string& filename);

    // распечатки для парсера
    void print_header();
    void print_error_msg(const Token& token, int state, Symbol sym);
    void print_success_msg();
    // void report_syntax_error(const Token& token, int state);

    // bool try_recovery(size_t& input_pos,
    //               std::vector<int>& state_stack,
    //               std::vector<Token>& token_stack,
    //               const std::vector<Token>& input,
    //               bool& has_error_flag);

    void execute_shift(std::vector<int>& state_stack,
                               std::vector<Token>& token_stack,
                               const Token& tok,
                               int next_state);


    bool execute_reduce(std::vector<int>& state_stack,
                                std::vector<Token>& token_stack,
                                int rule_index);
};
