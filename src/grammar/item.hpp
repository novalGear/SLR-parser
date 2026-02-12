#pragma once
#include "generated/symbol_enums.hpp"

struct Item {
    std::size_t rule_id;        // индекс правила в GRAMMAR_RULES
    std::size_t dot_pos;        // позиция точки (in [0, rule.length])
};

Symbol symbol_after_dot(const Item& item);
bool is_complete(const Item& item);


using ItemSet = std::set<Item>;

ItemSet initial_set();
ItemSet closure(const ItemSet& I);

ItemSet goto_items(const ItemSet& I, Symbol X);
