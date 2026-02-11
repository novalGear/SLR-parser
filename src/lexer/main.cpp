#include <iostream>
#include <stdexcept>

#include "lexer.hpp"
#include "tokens.hpp"

int main() {
    try {
        std::cout << "Lexer start:\n";
        while (next_token() != Token::END_OF_FILE) {
            continue;
        }
        std::cout << "Done.\n";
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
