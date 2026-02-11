#pragma once

#include "generated/symbol_enums.hpp"

constexpr const char* symbol_name(Symbol s);
constexpr bool is_terminal(Symbol s);
constexpr int nonterminal_index(Symbol s);
constexpr int terminal_index(Symbol s);
