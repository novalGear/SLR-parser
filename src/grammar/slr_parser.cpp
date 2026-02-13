#include "slr_parser.hpp"
#include "table_builder.hpp"
#include "first_follow.hpp"

int SLR_Parser::init() {
    build_tables();
    return 0;
}

int SLR_Parser::build_tables() {
    FirstSet first = create_first_set();
    FollowSet follow_sets = create_follow_set(first);
    print_first_follow(first, follow_sets);

    TableBuilder builder;
    builder.build(first, follow_sets);

    states_         = builder.take_states();
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
