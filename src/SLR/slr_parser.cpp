#include "slr_parser.hpp"
#include "table_builder.hpp"
#include "../grammar/symbol_utils.hpp"
#include "../grammar/first_follow.hpp"
#include "../grammar/rule.hpp"
#include "../lexer/token_struct.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>

int SLR_Parser::init() {
    if (!load_from_json(DATA_DIR "/slr_cache.json")) {
        build_tables();
        std::cout << "Tables built" << "\n";
        save_to_json(DATA_DIR "/slr_cache.json");
        std::cout << "Config saved to " << (DATA_DIR "/slr_cache.json") << "\n";
    } else {
        std::cout << "Loaded config from " << (DATA_DIR "/slr_cache.json") << "\n";
    }
    return 0;
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


// --- Обновленная функция parse ---
bool SLR_Parser::parse(const std::vector<Token>& tokens) {
    std::vector<int> state_stack = {0};
    std::vector<Token> token_stack;

    std::vector<Token> input = tokens;

    // Добавляем EOF, если нет
    if (input.empty() || input.back().type != Symbol::END_MARKER) {
        input.push_back(Token::make(Symbol::END_MARKER, "$", 0));
    }

    size_t input_pos = 0;

    // Вектор символов только для удобного вывода входа (можно оптимизировать, обращаясь напрямую к token.type)
    std::vector<Symbol> input_symbols;
    input_symbols.reserve(input.size());
    for(const auto& t : input) {
        input_symbols.push_back(t.type); // Просто копируем Symbol
    }

    std::cout << "\n--- Parsing Started ---\n";
    std::cout << std::left
              << std::setw(10) << "State"
              << std::setw(15) << "Token"
              << std::setw(40) << "Stack"
              << " | " << std::setw(25) << "Input"
              << " | " << "Action" << std::endl;
    std::cout << "--------------------------------------------------------------------------------------------------\n";

    while (true) {
        int current_state = state_stack.back();

        if (input_pos >= input.size()) break;

        const Token& current_token_obj = input[input_pos];
        Symbol current_token_sym = current_token_obj.type; // Прямо берем Symbol
//
//         std::cout << "# [DEBUG] State: q" << current_state
//                   << ", Token: " << symbol_to_display_string(current_token_sym) << std::endl;

        Action action = get_action(current_state, current_token_sym);

        dump_parser_state(state_stack, token_stack, input_symbols, input_pos, action);

        switch (action.type) {
            case ActionType::SHIFT: {
                token_stack.push_back(current_token_obj);
                state_stack.push_back(action.value);
                input_pos++;
                break;
            }
            case ActionType::REDUCE: {
                const Rule& rule = GRAMMAR_RULES[action.value];

                for (size_t i = 0; i < rule.length; ++i) {
                    state_stack.pop_back();
                    token_stack.pop_back();
                }

                int next_state = get_goto(state_stack.back(), rule.lhs);
                if (next_state == -1) {
                    std::cerr << "Error: No GOTO...\n";
                    return false;
                }

                state_stack.push_back(next_state);

                // Создаем токен для нетерминала. Теперь это легко!
                Token non_term_tok;
                non_term_tok.type = rule.lhs; // Прямо кладем Symbol (нетерминал)
                non_term_tok.text = symbol_to_display_string(rule.lhs);
                non_term_tok.line = 0;
                token_stack.push_back(non_term_tok);
                break;
            }
            case ActionType::ACCEPT:
                std::cout << "--------------------------------------------------------------------------------------------------\n";
                std::cout << "Parsing Successful!\n";
                return true;
            case ActionType::ERROR:
                std::cout << "--------------------------------------------------------------------------------------------------\n";
                std::cerr << "Parse Error at token: " << symbol_to_display_string(current_token_sym)
                          << " in state q" << current_state << std::endl;
                return false;
        }
    }
    return false;
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

