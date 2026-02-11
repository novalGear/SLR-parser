#include "first_follow.hpp"
#include "rule.hpp"
#include "symbol_utils.hpp"


#include <set>
#include <iostream>

using SymbolSet = std::set<Symbol>;

// FIRST для всех символов
std::array<SymbolSet, SYMBOL_COUNT> first;

// FOLLOW множества для нетерминалов
std::array<SymbolSet, NONTERMINAL_COUNT> follow;

bool add_all(SymbolSet& destination, const SymbolSet& source) {
    size_t before = destination.size();
    destination.insert(source.begin(), source.end());
    return destination.size() > before;
}

void create_first_set() {
    // Инициализация терминалов
    for (size_t i = NONTERMINAL_COUNT; i < SYMBOL_COUNT; ++i) {
        first[i].insert(static_cast<Symbol>(i));
    }

    // Итеративное вычисление FIRST для нетерминалов
    bool changed = true;
    while (changed) {
        changed = false;

        for (size_t i = 0; i < GRAMMAR_RULES_COUNT; ++i) {
            const Rule& rule = GRAMMAR_RULES[i];
            if (rule.length == 0) continue;

            size_t lhs = static_cast<size_t>(rule.lhs);
            size_t first_sym = static_cast<size_t>(rule.rhs[0]);

            // FIRST(lhs) += FIRST(first_sym)
            changed |= add_all(first[lhs], first[first_sym]);
        }
    }
}

void create_follow_set() {

    // Инициализация follow
    for (auto& s : follow) s.clear();
    follow[nonterminal_index(START_SYMBOL)].insert(END_MARKER);

    bool changed = true;
    while(changed) {
        changed = false;
        for (size_t i = 0; i < GRAMMAR_RULES_COUNT; ++i) {
            const Rule& rule = GRAMMAR_RULES[i];
            if (rule.length == 0) continue;

            for (size_t sym_ind = 0; sym_ind < rule.length - 1; ++sym_ind) {
                Symbol curr = rule.rhs[sym_ind];
                if (is_terminal(curr)) continue;    // FOLLOW-set только для нетерминалов

                Symbol next = rule.rhs[sym_ind + 1];
                changed |= add_all(
                    follow[nonterminal_index(curr)],
                    first[static_cast<size_t>(next)]
                );
            }

            // добавление для заключительного символа (особый случай)
            Symbol last = rule.rhs[rule.length - 1];
            if (not is_terminal(last)) {
                changed |= add_all(
                    follow[static_cast<size_t>(last)],
                    follow[static_cast<size_t>(rule.lhs)]
                );
            }
        }
    }
}


void print_first_follow() {
    std::cout << "=== FIRST sets ===\n";
    for (size_t i = 0; i < NONTERMINAL_COUNT; ++i) {
        Symbol nt = static_cast<Symbol>(i);
        std::cout << "FIRST(" << SYMBOL_NAMES[i] << ") = { ";
        for (Symbol s : first[i]) {
            std::cout << SYMBOL_NAMES[static_cast<size_t>(s)] << " ";
        }
        std::cout << "}\n";
    }

    std::cout << "\n=== FOLLOW sets ===\n";
    for (size_t i = 0; i < NONTERMINAL_COUNT; ++i) {
        Symbol nt = static_cast<Symbol>(i);
        std::cout << "FOLLOW(" << SYMBOL_NAMES[i] << ") = { ";
        for (Symbol s : follow[i]) {
            std::cout << SYMBOL_NAMES[static_cast<size_t>(s)] << " ";
        }
        std::cout << "}\n";
    }
}
