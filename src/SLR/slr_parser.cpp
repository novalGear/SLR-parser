#include "slr_parser.hpp"
#include "table_builder.hpp"
#include "../grammar/symbol_utils.hpp"
#include "../grammar/first_follow.hpp"
#include "../grammar/rule.hpp"
#include "../lexer/token_struct.hpp"

#include <iomanip>
#include <sstream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>

SLR_Parser::SLR_Parser() {
    if (!load_from_json(DATA_DIR "/slr_cache.json")) {
        build_tables();
        std::cout << "Tables built" << "\n";
        save_to_json(DATA_DIR "/slr_cache.json");
        std::cout << "Config saved to " << (DATA_DIR "/slr_cache.json") << "\n";
    } else {
        std::cout << "Loaded config from " << (DATA_DIR "/slr_cache.json") << "\n";
    }
}

int SLR_Parser::build_tables() {
    FirstSet first = create_first_set();
    FollowSet follow_sets = create_follow_set(first);
    print_first_follow(first, follow_sets);

    TableBuilder builder;
    builder.build(follow_sets);

    // states_         = builder.take_states();
    action_table_   = builder.take_action_table();
    goto_table_     = builder.take_goto_table();
    return 0;
}

const Action& SLR_Parser::get_action(size_t state, Symbol terminal) const {
    int t_idx = terminal_index(terminal);
    if (t_idx == -1 || state >= action_table_.size()) {
        static Action error{ActionType::ERROR, 0};
        return error;
    }
    return action_table_[state][t_idx];
}

int SLR_Parser::get_goto(size_t state, Symbol nonterminal) const {
    int nt_idx = nonterminal_index(nonterminal);
    if (nt_idx == -1 || state >= goto_table_.size()) {
        return -1;
    }
    return goto_table_[state][nt_idx];
}

void SLR_Parser::print_header() {
    std::cout << "\n--- Parsing Started ---\n";
    std::cout << std::left
              << std::setw(10) << "State"
              << std::setw(15) << "Token"
              << std::setw(40) << "Stack"
              << " | " << std::setw(25) << "Input"
              << " | " << "Action" << std::endl;
    std::cout << "--------------------------------------------------------------------------------------------------\n";
}

void SLR_Parser::print_success_msg() {
    std::cout << "--------------------------------------------------------------------------------------------------\n";
    std::cout << "\033[32mParsing Successful!\033[0m\n"; // Зеленый цвет
}

void SLR_Parser::print_error_msg(const Token& token, int state, Symbol sym) {
    std::cout << "--------------------------------------------------------------------------------------------------\n";
    std::cerr << "\n\033[1;31m[SYNTAX ERROR]\033[0m at Line " << token.line
              << ", Column " << token.column << ":\n";
    std::cerr << "  Unexpected token: '" << token.text << "' ("
              << symbol_to_display_string(sym) << ")\n";
    std::cerr << "  Parser state: q" << state << "\n";
}

bool SLR_Parser::execute_reduce(std::vector<int>& state_stack,
                                std::vector<Token>& token_stack,
                                int rule_index) {
    const Rule& rule = GRAMMAR_RULES[rule_index];

    // Проверка на целостность стека
    if (token_stack.size() < rule.length) {
        std::cerr << "\n[INTERNAL ERROR] Stack underflow during Reduce!\n";
        std::cerr << "  Rule length: " << rule.length
                  << ", Stack size: " << token_stack.size() << "\n";
        return false;
    }

    for (size_t i = 0; i < rule.length; ++i) {
        state_stack.pop_back();
        token_stack.pop_back();
    }

    int next_state = get_goto(state_stack.back(), rule.lhs);
    if (next_state == -1) {
        std::cerr << "\n[INTERNAL ERROR] No GOTO transition for state "
                  << state_stack.back() << " and symbol "
                  << symbol_to_display_string(rule.lhs) << "\n";
        return false;
    }

    state_stack.push_back(next_state);

    // Создаем токен для нетерминала (левая часть правила)
    Token non_term_tok;
    non_term_tok.type = rule.lhs;
    non_term_tok.text = symbol_to_display_string(rule.lhs);
    // Наследуем позицию от последнего оставшегося элемента стека для точности ошибок в будущем
    non_term_tok.line = token_stack.empty() ? 0 : token_stack.back().line;
    non_term_tok.column = 0;

    token_stack.push_back(non_term_tok);

    return true;
}

bool SLR_Parser::parse(const std::vector<Token>& tokens) {
    std::vector<int> state_stack = {0};
    std::vector<Token> token_stack;

    std::vector<Token> input = tokens;

    if (input.empty() || input.back().type != Symbol::END_MARKER) {
        input.push_back(Token::make(Symbol::END_MARKER, "$", 0));
    }

    size_t input_pos = 0;
    std::vector<Symbol> input_symbols;
    input_symbols.reserve(input.size());
    for(const auto& t : input) {
        input_symbols.push_back(t.type);
    }

    print_header();
    while (true) {
        if (input_pos >= input.size()) { break; }

        int current_state = state_stack.back();
        const Token& current_token = input[input_pos];
        Symbol current_sym = current_token.type;

        Action action = get_action(current_state, current_sym);
        dump_parser_state(state_stack, token_stack, input_symbols, input_pos, action);
        switch (action.type) {
            case ActionType::SHIFT: {
                token_stack.push_back(current_token);
                state_stack.push_back(action.value);
                input_pos++;
                break;
            }
            case ActionType::REDUCE: {
                if (!execute_reduce(state_stack, token_stack, action.value)) {
                    return false; // Внутренняя ошибка
                }
                break;
            }
            case ActionType::ACCEPT: {
                print_success_msg();
                return true;
            }
            case ActionType::ERROR: {
                print_error_msg(current_token, current_state, current_sym);
                return false;
            }
        }
    }

    std::cerr << "\n[INTERNAL ERROR] Parser terminated unexpectedly without Accept.\n";
    return false;
}
