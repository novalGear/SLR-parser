#ifndef FIRST_FOLLOW_HPP
#define FIRST_FOLLOW_HPP

#include <array>
#include <set>
#include "generated/symbol_enums.hpp"

using SymbolSet = std::set<Symbol>;

using FirstSet  = std::array<SymbolSet, SYMBOL_COUNT>;
using FollowSet = std::array<SymbolSet, NONTERMINAL_COUNT>;

FirstSet create_first_set();
FollowSet create_follow_set(FirstSet first);
void print_first_follow(FirstSet first, FollowSet follow); // для отладки

#endif
