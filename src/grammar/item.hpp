#pragma once
#include "generated/symbol_enums.hpp"

#include <set>

struct Item {
    std::size_t rule_id;        // индекс правила в GRAMMAR_RULES
    std::size_t dot_pos;        // позиция точки (in [0, rule.length])

    // для итерации по Item в контейнерах
    bool operator<(const Item& other) const {
        if (rule_id != other.rule_id) return rule_id < other.rule_id;
        return dot_pos < other.dot_pos;
    }
};

using ItemSet = std::set<Item>;

Symbol symbol_after_dot(const Item& item);
bool is_complete(const Item& item);
std::set<Symbol> get_next_symbols(const ItemSet& item_set);

// Возвращает множество начальных пунктов - всех грамматических правил с точкой в начале
ItemSet compute_initial_items_set();

ItemSet closure(const ItemSet& I);
ItemSet goto_items(const ItemSet& I, Symbol X);

// отладка
void print_item(const Item& item);
