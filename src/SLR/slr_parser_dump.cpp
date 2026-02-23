#include "slr_parser.hpp"
#include "table_builder.hpp"
#include "../grammar/symbol_utils.hpp"
#include "../grammar/first_follow.hpp"
#include "../grammar/rule.hpp"

#include <iomanip>
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>

void SLR_Parser::dump_action_table_csv(const std::string& filename) const {
    std::ofstream file(filename);
    // Заголовок
    file << "State";
    for (size_t i = NONTERMINAL_COUNT; i < SYMBOL_COUNT; ++i) {
        if (static_cast<Symbol>(i) != Symbol::END_MARKER) {
            file << "," << symbol_name(static_cast<Symbol>(i));
        }
    }
    file << "," << symbol_name(Symbol::END_MARKER) << "\n";

    // Строки
    for (size_t state = 0; state < action_table_.size(); ++state) {
        file << state;
        for (size_t i = NONTERMINAL_COUNT; i < SYMBOL_COUNT; ++i) {
            Symbol sym = static_cast<Symbol>(i);
            if (sym != Symbol::END_MARKER) {
                file << "," << format_action(action_table_[state][terminal_index(sym)]);
            }
        }
        // END_MARKER
        file << "," << format_action(action_table_[state][terminal_index(Symbol::END_MARKER)]) << "\n";
    }
}

std::string format_action(const Action& act) {
    switch (act.type) {
        case ActionType::SHIFT:
            return "S" + std::to_string(act.value);
        case ActionType::REDUCE:
            return "R" + std::to_string(act.value);
        case ActionType::ACCEPT:
            return "A";
        case ActionType::ERROR:
        default:
            return "ERR";
    }
}

Action parse_action(const std::string& str) {
    if (str.empty()) {
        return {ActionType::ERROR, 0};
    }
    if (str == "A") {
        return {ActionType::ACCEPT, 0};
    }
    if (str[0] == 'S') {
        size_t value = std::stoul(str.substr(1));
        return {ActionType::SHIFT, static_cast<int>(value)};
    }
    if (str[0] == 'R') {
        size_t value = std::stoul(str.substr(1));
        return {ActionType::REDUCE, static_cast<int>(value)};
    }
    return {ActionType::ERROR, -1};
}


void SLR_Parser::save_to_json(const std::string& filename) const {
    nlohmann::json j;

    // Количество состояний
    j["num_states"] = action_table_.size();

    // ACTION таблица
    j["action_table"] = nlohmann::json::array();
    for (const auto& row : action_table_) {
        nlohmann::json action_row = nlohmann::json::array();
        for (const Action& act : row) {
            action_row.push_back(format_action(act));
        }
        j["action_table"].push_back(action_row);
    }

    // GOTO таблица
    j["goto_table"] = nlohmann::json::array();
    for (const auto& row : goto_table_) {
        nlohmann::json goto_row = nlohmann::json::array();
        for (int state : row) {
            goto_row.push_back(state);
        }
        j["goto_table"].push_back(goto_row);
    }

    std::ofstream file(filename);
    file << j.dump(2);
}

bool SLR_Parser::load_from_json(const std::string& filename) {
    if (!std::filesystem::exists(filename)) {
        return false;
    }

    std::ifstream file(filename);
    nlohmann::json j;
    file >> j;

    size_t num_states = j["num_states"];

    // Загрузка ACTION
    action_table_.clear();
    for (const auto& row : j["action_table"]) {
        std::vector<Action> action_row;
        for (const std::string& act_str : row) {
            action_row.push_back(parse_action(act_str));
        }
        action_table_.push_back(action_row);
    }

    // Загрузка GOTO
    goto_table_.clear();
    for (const auto& row : j["goto_table"]) {
        std::vector<int> goto_row;
        for (int state : row) {
            goto_row.push_back(state);
        }
        goto_table_.push_back(goto_row);
    }

    return true;
}

void SLR_Parser::dump_parser_state(
    const std::vector<int>& state_stack,
    const std::vector<Token>& token_stack,
    const std::vector<Symbol>& input,
    size_t input_pos,
    const Action& action
) {
    std::ostringstream stack_oss;
    std::ostringstream input_oss;

    // 1. Стек ТОЛЬКО символов (без состояний)
    stack_oss << "$";
    for (const auto& tok : token_stack) {
        stack_oss << " " << symbol_to_display_string(tok.type); // Прямо из поля type
    }

    // 2. Вход
    for (size_t i = input_pos; i < input.size(); ++i) {
        if (i == input.size() - 1 && input[i] == Symbol::END_MARKER) {
             input_oss << "$";
        } else {
             input_oss << symbol_to_display_string(input[i]);
        }
    }
    if (input_pos >= input.size()) input_oss << "$";

    // 3. Действие
    std::string action_str;
    switch (action.type) {
        case ActionType::SHIFT:
            action_str = "Shift";
            break;
        case ActionType::REDUCE: {
            const Rule& rule = GRAMMAR_RULES[action.value];

            std::string rhs_str = "";
            if (rule.length == 0) {
                rhs_str = "ε";
            } else {
                size_t start_idx = token_stack.size();
                if (start_idx >= rule.length) {
                    start_idx -= rule.length;
                    for (size_t k = 0; k < rule.length; ++k) {
                        rhs_str += symbol_to_display_string(token_stack[start_idx + k].type);
                    }
                } else {
                    rhs_str = "[Err]";
                }
            }
            action_str = "Reduce " + symbol_to_display_string(rule.lhs) + " -> " + rhs_str;
            break;
        }
        case ActionType::ACCEPT:
            action_str = "Accept";
            break;
        case ActionType::ERROR:
            action_str = "Error";
            break;
        default:
            action_str = "Unknown";
    }

    std::cout << std::left
              << std::setw(10) << ""
              << std::setw(15) << ""
              << std::setw(40) << stack_oss.str()
              << " | " << std::setw(25) << input_oss.str()
              << " | " << action_str << std::endl;
}
