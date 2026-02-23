#include <iostream>
#include <vector>
#include <string>
#include <FlexLexer.h>

#include "../lexer/lexer_defs.hpp"
#include "../lexer/token_struct.hpp"
#include "slr_parser.hpp"

int main() {
    std::cout << "=== SLR Parser with Lexical Analysis ===\n";
    std::cout << "Enter expression (end with Ctrl+D on Linux/Mac or Ctrl+Z on Windows):\n";

    yyFlexLexer lexer;
    lexer.switch_streams(&std::cin, &std::cerr);
    std::vector<Token> tokens;

    while (true) {
        int token_id = lexer.yylex();
        if (token_id == TOK_ERROR) {
            std::cerr << "\n[Lexer Error] Stopping due to lexical error.\n";
            return 1;
        }

        const Token& current_token = yylval;
        tokens.push_back(current_token);
        if (current_token.type == Symbol::END_MARKER) {
            break;
        }
    }

    std::cout << "\n--- Tokens Read: " << tokens.size() << " ---\n";

    SLR_Parser parser;
    bool result = parser.parse(tokens);

    if (result) {
        std::cout << "\n[Success] Input parsed correctly!\n";
        return 0;
    } else {
        std::cout << "\n[Failure] Parsing failed.\n";
        return 1;
    }
}
