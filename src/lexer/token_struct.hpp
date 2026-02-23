#pragma once

#include "../grammar/symbol_utils.hpp"
#include <string>
#include <variant>
#include <cstdint>

using TokenValue = std::variant<std::monostate, double, std::string>;

struct Token {
    Symbol type;
    TokenValue value;
    std::string text;
    size_t line;
    size_t column;

    Token() : type(Symbol::UNKNOWN), line(0), column(0), value(std::monostate{}) {}

    static Token make(Symbol s, const std::string& txt, size_t l, size_t c = 0) {
        Token tok;
        tok.type = s;
        tok.text = txt;
        tok.line = l;
        tok.column = c;

        if (s == Symbol::NUMBER) {
            try { tok.value = std::stod(txt); } catch (...) { tok.value = 0.0; }
        } else if (s == Symbol::VAR) {
            tok.value = txt;
        } else {
            tok.value = std::monostate{};
        }
        return tok;
    }

    bool isNumber() const { return std::holds_alternative<double>(value); }
    bool isIdentifier() const { return std::holds_alternative<std::string>(value); }

    double getNumber() const {
        if (!isNumber()) return 0.0;
        return std::get<double>(value);
    }

    const std::string& getIdentifier() const {
        if (!isIdentifier()) {
            static const std::string empty;
            return empty;
        }
        return std::get<std::string>(value);
    }

    Symbol toSymbol() const {
        return type;
    }

    bool isTerminal() const {
        return is_terminal(type);
    }
};
