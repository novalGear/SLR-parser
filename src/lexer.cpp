#include <iostream>
#include "tokens.hpp"
#include "token_utils.hpp"

extern "C" {
    int yylex();
}

Token next_token() {
    int tok = yylex();
    if (tok == -1) {
        throw std::runtime_error("Lexical Error");
    }
    Token token = static_cast<Token>(tok);
    std::cout << token_name(token) << "(" << tok << ")\n";
    return token;
}
