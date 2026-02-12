#pragma once

#include "item.hpp"

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
    const Action& get_action(int state, Symbol terminal) const;
    int get_goto(int state, Symbol nonterminal) const;

    int action(ItemSet state, Symbol symbol);
    int goto_state(ItemSet state, Symbol nonterminal);

    // debug
    SLR_ERROR get_last_error();
    void dump_tables();

private:
    // ожидается релиз:
    // стэк состояний
    // входная строка
    //


    struct State {
        ItemSet items;
        int id;
    };

    ActionTable action_table;
    GotoTable   goto_table;

    std::set<Symbol> get_next_symbols(const ItemSet& item_set);

    ItemSet closure(const ItemSet& I);
    ItemSet goto_items(const ItemSet& I, Symbol X);

    int build_dfa();
    int build_tables();
    SLR_ERROR errno;
};
