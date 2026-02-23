// token_type.hpp
#pragma once

#include "../grammar/generated/symbol_enums.hpp"

// Токены - это только то, что возвращает лексер (Терминалы)
enum class TokenType {
    PLUS,
    MINUS,
    STAR,
    SLASH,
    LPAREN,
    RPAREN,
    NUMBER,
    VAR,
    END_OF_FILE,
    ERROR
};

// Функция преобразования TokenType -> Symbol (для передачи в парсер)
// Парсер оперирует Symbol, поэтому нам нужно уметь превращать токен в символ грамматики
inline Symbol token_type_to_symbol(TokenType t) {
    switch (t) {
        case TokenType::PLUS:       return Symbol::PLUS;
        case TokenType::MINUS:      return Symbol::MINUS;
        case TokenType::STAR:       return Symbol::STAR;
        case TokenType::SLASH:      return Symbol::SLASH;
        case TokenType::LPAREN:     return Symbol::LPAREN;
        case TokenType::RPAREN:     return Symbol::RPAREN;
        case TokenType::NUMBER:     return Symbol::NUMBER;
        case TokenType::VAR:        return Symbol::VAR;
        case TokenType::END_OF_FILE:return Symbol::END_MARKER;
        default:                    return Symbol::UNKNOWN;
    }
}

// Обратное преобразование (если нужно для отладки)
inline TokenType symbol_to_token_type(Symbol s) {
    switch (s) {
        case Symbol::PLUS:       return TokenType::PLUS;
        case Symbol::MINUS:      return TokenType::MINUS;
        case Symbol::STAR:       return TokenType::STAR;
        case Symbol::SLASH:      return TokenType::SLASH;
        case Symbol::LPAREN:     return TokenType::LPAREN;
        case Symbol::RPAREN:     return TokenType::RPAREN;
        case Symbol::NUMBER:     return TokenType::NUMBER;
        case Symbol::VAR:        return TokenType::VAR;
        case Symbol::END_MARKER: return TokenType::END_OF_FILE;
        default:                 return TokenType::ERROR;
    }
}
