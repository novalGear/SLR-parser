#include "rules.hpp"
#include "symbol_utils.hpp"

using SymbolSet = std::set<Symbol>;

// FIRST для всех символов
std::array<SymbolSet, SYMBOL_COUNT> first;

// FOLLOW множества для нетерминалов
std::array<SymbolSet, NONTERMINAL_COUNT> follow;

void create_first_set() {

    // Инициализация терминалов
    for (int index = NONTERMINAL_COUNT; index < first.size(); ++index) {
        first[index].insert(static_cast<Symbol>(index));
    }

    // Итеративное вычисление FIRST для нетерминалов
    bool changed = true;
    while (changed) {
        changed = false;

        for (int rule_index = GRAMMAR_RULES_COUNT - 1; rule_index >= 0; --rule_index) {
            const Rule& rule = GRAMMAR_RULES[rule_index];
            if (rule.length == 0) continue;

            Symbol lhs = rule.lhs;
            Symbol first_sym = current_rule.rhs[0];

            // FIRST(lhs).insert(FIRST(first_sym);

//             auto& lhs_set = first[static_cast<size_t>(lhs)];
//             auto& rhs_set = first[static_cast<size_t>(first_sym)];
//
//             size_t before = lhs_set.size();
//             lhs_set.insert(rhs_set.begin(), rhs_set.end());
//             if (lhs_set.size() > before) {
//                 changed = true;
//             }

            for (Symbol s : first[static_cast<int>(first_sym)]) {
                auto result = first[static_cast<int>(lhs)].insert(s);
                if (result.second) {
                    changed = true;
                }
            }
        }
    }
}

void create_follow_set() {
    for (int rule_index = 0; rule_index < GRAMMAR_RULES_COUNT; ++rule_index) {
        Rule current_rule = GRAMMAR_RULES[rule_index];
        const Symbol* seq = current_rule.rhs;

        for (int sym_ind = 0; sym_ind < current_rule.length - 1; ++sym_ind) {
            int curr_sym = static_cast<int>(seq[sym_ind]);
            int next_sym = static_cast<int>(seq[sym_ind + 1]);
            follow[curr_sym].insert(first[next_sym]);
        }
    }
}

