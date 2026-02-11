// Сгенерировано из ../data/grammar.txt
#ifndef SYMBOL_ENUMS_HPP
#define SYMBOL_ENUMS_HPP

#include <cstddef>

enum class Symbol {
  E,
  P,
  T,
  START,
  PLUS,
  MINUS,
  STAR,
  SLASH,
  LPAREN,
  RPAREN,
  NUMBER,
  VAR,
  END_MARKER,
};

constexpr Symbol START_SYMBOL = Symbol::START;
constexpr Symbol END_MARKER = Symbol::END_MARKER;
constexpr std::size_t SYMBOL_COUNT = 13;
constexpr std::size_t NONTERMINAL_COUNT = 4;
constexpr std::size_t TERMINAL_COUNT = 9;

inline constexpr const char* SYMBOL_NAMES[SYMBOL_COUNT] = {
  "E", "P", "T", "START", "PLUS", "MINUS", "STAR", "SLASH", "LPAREN", "RPAREN", "NUMBER", "VAR", "END_MARKER"
};

#endif // SYMBOL_ENUMS_HPP