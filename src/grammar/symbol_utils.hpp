#pragma once

#include "generated/symbol_enums.hpp"

const char* symbol_name(Symbol s);
bool is_terminal(Symbol s);
int nonterminal_index(Symbol s);
int terminal_index(Symbol s);
