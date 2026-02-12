#include "item.hpp"
#include "rule.hpp"

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

using ItemSet = std::set<Item>;

ItemSet closure(const ItemSet& I) {
    ItemSet result = I;
    bool changed = true;
    while(changed) {
        changed = false;
        for (const Item& item: result) {
            Symbol sym = symbol_after_dot(item);
            if (not is_terminal(sym)) {
                // Раскрываем нетерминалы X, добавляя правила
                // вида X -> . ... (с точкой в начале)
                for (size_t rule_ind = 0; rule_ind < GRAMMAR_RULES_COUNT; ++rule_ind) {
                    if (GRAMMAR_RULES[rule_ind].lhs == sym) {
                        Item new_item{rule_ind, 0};
                        // добавляем и с нетерминалами в начале, эти пункты
                        // замыкаются на следующих итерациях
                        if (result.insert(new_item).second) {
                            changed = true;
                        }
                    }
                }
            }
        }
    }
    return result;
}


// TODO: test
ItemSet initial_set() {
    ItemSet I;
    for (size_t rule_ind = 0; rule_ind < GRAMMAR_RULES_COUNT; ++rule_ind) {
        Rule& rule = GRAMMAR_RULES[rule_ind];
        if (rule.length == 0) continue;
        I.insert({rule_ind, 0});
    }
}

// TODO: test
ItemSet complete_set() {
    return closure(initial_set());
};

ItemSet goto_items(const ItemSet& I, Symbol X) {
    ItemSet moved;
    for (const Item& item: I) {
        if (symbol_after_dot(item) == X) {
            moved.insert({item.rule_id, item.dot_pos + 1});
        }
    }
    return moved;
}
