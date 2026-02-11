#pragma once

#include <cstddef>
#include "generated/symbol_enums.hpp"

struct Rule {
    std::size_t id;             // Уникальный номер
    Symbol lhs;                 // Левая часть (нетерминал)
    const Symbol* rhs;          // Правая часть (список символов)
    std::size_t length;         // размер правой части
};

extern constexpr Rule* GRAMMAR_RULES;
extern const std::size_t GRAMMAR_RULES_COUNT;
