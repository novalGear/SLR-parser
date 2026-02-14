#include "slr_parser.hpp"
#include "table_builder.hpp"
#include "../grammar/symbol_utils.hpp"
#include "../grammar/first_follow.hpp"

#include <fstream>

int SLR_Parser::init() {
    build_tables();
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
    dump_action_table_csv("./data/actions.csv");
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


void SLR_Parser::dump_action_table_csv(const std::string& filename) const {
    std::ofstream file(filename);
    // Заголовок
    file << "State";
    for (size_t i = NONTERMINAL_COUNT; i < SYMBOL_COUNT; ++i) {
        if (static_cast<Symbol>(i) != END_MARKER) {
            file << "," << symbol_name(static_cast<Symbol>(i));
        }
    }
    file << "," << symbol_name(END_MARKER) << "\n";

    // Строки
    for (size_t state = 0; state < action_table_.size(); ++state) {
        file << state;
        for (size_t i = NONTERMINAL_COUNT; i < SYMBOL_COUNT; ++i) {
            Symbol sym = static_cast<Symbol>(i);
            if (sym != END_MARKER) {
                file << "," << format_action(action_table_[state][terminal_index(sym)]);
            }
        }
        // END_MARKER
        file << "," << format_action(action_table_[state][terminal_index(END_MARKER)]) << "\n";
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
            return "";
    }
}
