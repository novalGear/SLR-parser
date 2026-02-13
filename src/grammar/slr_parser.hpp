#pragma once

#include "item.hpp"
#include "first_follow.hpp"
#include <vector>
#include <map>

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
    size_t value;
};

struct State {
    ItemSet items;                          // набор пунктов состояния
    std::map<Symbol, size_t> transitions;   // X -> next_state_id
};


// [state][terminal]
using ActionTable = std::vector<std::vector<Action>>;

// [state][nonterminal]
using GotoTable = std::vector<std::vector<int>>;

class SLR_Parser {
public:
    SLR_Parser();

    // построение таблиц
    int init();

    // интерфейс парсера
    // const Action& get_action(int state, Symbol terminal) const;
    // int get_goto(int state, Symbol nonterminal) const;

    // debug
    // SLR_Error get_last_error();
    // void dump_tables();

private:
    // ожидается релиз:
    // стэк состояний
    // входная строка
    //
    std::vector<State> states_;
    ActionTable action_table_;
    GotoTable   goto_table_;

    int build_dfa();
    int build_tables();

    SLR_Error errno;
};
