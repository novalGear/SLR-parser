#include "generated/symbol_enums.hpp"
#include <string>

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

std::string symbol_to_display_string(Symbol s) {
    switch (s) {
        case Symbol::PLUS:       return "+";
        case Symbol::MINUS:      return "-";
        case Symbol::STAR:       return "*";
        case Symbol::SLASH:      return "/";
        case Symbol::LPAREN:     return "(";
        case Symbol::RPAREN:     return ")";
        case Symbol::NUMBER:     return "num"; // Или конкретное значение, если передавать токен
        case Symbol::VAR:        return "id";  // Или конкретное имя
        case Symbol::END_MARKER: return "$";

        // Нетерминалы можно оставлять как есть или сокращать
        case Symbol::E:          return "E";
        case Symbol::T:          return "T";
        case Symbol::P:          return "P";
        case Symbol::START:      return "S'";

        default:                 return "?";
    }
}
