#ifndef FIRST_FOLLOW_HPP
#define FIRST_FOLLOW_HPP

#include <array>
#include <set>
#include "generated/symbol_enums.hpp"

using SymbolSet = std::set<Symbol>;

extern std::array<SymbolSet, SYMBOL_COUNT> first;
extern std::array<SymbolSet, NONTERMINAL_COUNT> follow;

void create_first_set();
void create_follow_set();
void print_first_follow(); // для отладки

#endif
