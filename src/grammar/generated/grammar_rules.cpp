// Сгенерировано из ../data/grammar.txt
#include "../rule.hpp"

static constexpr Symbol RHS_0[] = { Symbol::E };
static constexpr Symbol RHS_1[] = { Symbol::E, Symbol::PLUS, Symbol::T };
static constexpr Symbol RHS_2[] = { Symbol::E, Symbol::MINUS, Symbol::T };
static constexpr Symbol RHS_3[] = { Symbol::T };
static constexpr Symbol RHS_4[] = { Symbol::T, Symbol::STAR, Symbol::P };
static constexpr Symbol RHS_5[] = { Symbol::T, Symbol::SLASH, Symbol::P };
static constexpr Symbol RHS_6[] = { Symbol::P };
static constexpr Symbol RHS_7[] = { Symbol::NUMBER };
static constexpr Symbol RHS_8[] = { Symbol::VAR };
static constexpr Symbol RHS_9[] = { Symbol::LPAREN, Symbol::E, Symbol::RPAREN };

const Rule GRAMMAR_RULES_ARRAY[] = {
  {0, Symbol::START, RHS_0, 1},
  {1, Symbol::E, RHS_1, 3},
  {2, Symbol::E, RHS_2, 3},
  {3, Symbol::E, RHS_3, 1},
  {4, Symbol::T, RHS_4, 3},
  {5, Symbol::T, RHS_5, 3},
  {6, Symbol::T, RHS_6, 1},
  {7, Symbol::P, RHS_7, 1},
  {8, Symbol::P, RHS_8, 1},
  {9, Symbol::P, RHS_9, 3}
};

const Rule* const GRAMMAR_RULES = GRAMMAR_RULES_ARRAY;
const std::size_t GRAMMAR_RULES_COUNT = sizeof(GRAMMAR_RULES_ARRAY) / sizeof(Rule);
