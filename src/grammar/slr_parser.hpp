#pragma once

#include "item.hpp"
#include "first_follow.hpp"
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
    SLR_Parser() = default;

    // построение таблиц
    int init();

    // интерфейс парсера
    const Action& get_action(size_t state, Symbol terminal) const;
    int get_goto(size_t state, Symbol nonterminal) const;

    // debug

    void dump_action_table_csv(const std::string& filename) const;
    // SLR_Error get_last_error();
    // void dump_tables();

private:
    // ожидается релиз:
    // стэк состояний
    // входная строка
    //
    // std::vector<State> states_;
    ActionTable action_table_;
    GotoTable   goto_table_;

    int build_dfa();
    int build_tables();

    SLR_Error errno;
};
