#include "item.hpp"
#include "rule.hpp"
#include "symbol_utils.hpp"

#include <iostream>

Symbol symbol_after_dot(const Item& item) {
    const Rule& rule = GRAMMAR_RULES[item.rule_id];
    if (item.dot_pos == rule.length) {
        return Symbol::END_MARKER;
    }
    return rule.rhs[item.dot_pos];
}

bool is_complete(const Item& item) {
    return item.dot_pos == GRAMMAR_RULES[item.rule_id].length;
}

std::set<Symbol> get_next_symbols(const ItemSet& item_set) {
    std::set<Symbol> result;
    for (const Item& item : item_set) {
        if (!is_complete(item)) {
            result.insert(symbol_after_dot(item));
        }
    }
    return result;
}


ItemSet closure(const ItemSet& I) {
    ItemSet result = I;
    bool changed = true;
    while(changed) {
        changed = false;
        ItemSet new_items;
        for (const Item& item: result) {
            if (!is_complete(item)) {
                Symbol next_symbol = symbol_after_dot(item);
                if (not is_terminal(next_symbol)) {
                    // Раскрываем нетерминалы X, добавляя правила
                    // вида X -> . ... (с точкой в начале)
                    for (size_t rule_ind = 0; rule_ind < GRAMMAR_RULES_COUNT; ++rule_ind) {
                        if (GRAMMAR_RULES[rule_ind].lhs == next_symbol) {
                            Item new_item{rule_ind, 0};
                            // добавляем и с нетерминалами в начале, эти пункты
                            // замыкаются на следующих итерациях
                            if (result.find(new_item) == result.end()) {
                                new_items.insert(new_item);
                                changed = true;
                            }
                        }
                    }
                }
            }
        }
        result.insert(new_items.begin(), new_items.end());
    }
    return result;
}


// TODO: test
ItemSet compute_initial_items_set() {
    ItemSet I;
    for (size_t rule_ind = 0; rule_ind < GRAMMAR_RULES_COUNT; ++rule_ind) {
        const Rule& rule = GRAMMAR_RULES[rule_ind];
        if (rule.length == 0) continue;
        I.insert({rule_ind, 0});
    }
    return I;
}

ItemSet goto_items(const ItemSet& I, Symbol X) {
    ItemSet moved;
    for (const Item& item: I) {
        if (symbol_after_dot(item) == X) {
            moved.insert({item.rule_id, item.dot_pos + 1});
        }
    }
    return moved;
}

void print_item(const Item& item) {
    const Rule& rule = GRAMMAR_RULES[item.rule_id];
    std::cout << symbol_name(rule.lhs) << " -> ";

    for (size_t i = 0; i < rule.length; ++i) {
        if (i == item.dot_pos) std::cout << "• ";
        std::cout << symbol_name(rule.rhs[i]) << " ";
    }
    if (item.dot_pos == rule.length) std::cout << "• ";
    std::cout << "\n";
}
