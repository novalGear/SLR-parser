#include "generated/symbol_enums.hpp"

const char* symbol_name(Symbol s) {
    return SYMBOL_NAMES[static_cast<std::size_t>(s)];
}

bool is_terminal(Symbol s) {
    return static_cast<std::size_t>(s) >= NONTERMINAL_COUNT;
}

int nonterminal_index(Symbol s) {
    return is_terminal(s) ? -1 : static_cast<int>(s);
}

int terminal_index(Symbol s) {
    auto idx = static_cast<std::size_t>(s);
    return (idx < NONTERMINAL_COUNT) ? -1 : static_cast<int>(idx - NONTERMINAL_COUNT);
}
