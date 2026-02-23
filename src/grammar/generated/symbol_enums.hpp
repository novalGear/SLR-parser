// symbol_enums.hpp
#ifndef SYMBOL_ENUMS_HPP
#define SYMBOL_ENUMS_HPP

#include <cstddef>
#include <string_view>

// Символы грамматики: Терминалы + Нетерминалы
enum class Symbol {
  // Нетерминалы (используются только внутри парсера)
  E,
  P,
  T,
  START,

  // Терминалы (совпадают с токенами, но это часть грамматики)
  PLUS,
  MINUS,
  STAR,
  SLASH,
  LPAREN,
  RPAREN,
  NUMBER,
  VAR,

  // Специальные
  END_MARKER,
  UNKNOWN // Для ошибок
};

constexpr std::size_t SYMBOL_COUNT = 14;
constexpr std::size_t NONTERMINAL_COUNT = 4; // E, P, T, START
constexpr std::size_t TERMINAL_COUNT = 10;   // PLUS...VAR + END_MARKER? (уточните логику подсчета)

inline constexpr const char* SYMBOL_NAMES[] = {
  "E", "P", "T", "START",
  "PLUS", "MINUS", "STAR", "SLASH", "LPAREN", "RPAREN", "NUMBER", "VAR",
  "END_MARKER", "UNKNOWN"
};

// Helper для отладки
inline std::string_view to_string(Symbol s) {
    if (static_cast<size_t>(s) < SYMBOL_COUNT) {
        return SYMBOL_NAMES[static_cast<size_t>(s)];
    }
    return "INVALID_SYMBOL";
}

#endif
