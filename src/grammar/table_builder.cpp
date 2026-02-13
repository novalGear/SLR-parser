#include "table_builder.hpp"

void TableBuilder::build(const FollowSet& follow_sets) {
    build_states();
    fill_tables(follow_sets);
}

void TableBuilder::build_states() {

    ItemSet initial = compute_initial_items_set();
    states_.push_back({initial, {}});
    state_to_id_[initial] = 0;

    std::queue<size_t> q; // здесь накапливаем индексы необработанных состояний
    q.push(0);

    // добавляем все получаемые через goto состояния в очередь,
    // на каждой итерации while работаем с одним состоянием

    while(!q.empty()) {
        size_t state_id = q.front();
        q.pop();
        State& curr_state = states_[state_id];
        std::set<Symbol> successor_symbols = get_next_symbols(curr_state.items);

        for (Symbol symbol : successor_symbols) {
            ItemSet target_items = closure(goto_items(curr_state.items, symbol));

            auto it = state_to_id_.find(target_items);
            if (it == state_to_id_.end()) {
                // new state
                size_t new_state_id = states_.size();
                states_.push_back({target_items, {}});
                state_to_id_[target_items] = new_state_id;
                curr_state.transitions[symbol]  = new_state_id;
                q.push(new_state_id);
            } else {
                // уже существующее состояние
                curr_state.transitions[symbol] = it->second;
            }
        }
    }
}

void TableBuilder::fill_tables(const FollowSet& follow_sets) {
    // init actions, goto tables
    size_t states_count = states_.size();
    action_table_.assign(states_count, std::vector<Action>(TERMINAL_COUNT, {ActionType::ERROR, -1}));
    goto_table.assign(states_count, std::vector<int>(NONTERMINAL_COUNT, -1));

    for (size_t state_id = 0; state_id < states_count; ++state_id) {
        const State& curr_state = states_[state_id];
        for (const Item& item : curr_state.items) {
            if (is_complete(item)) {
                const Rule& rule = GRAMMAR_RULES[item.rule_id];
                if (rule.lhs == START_SYMBOL) {
                    add_accept(state_id);
                } else {
                    add_reduce(state_id, follow_sets, item);
                }
            } else {
                Symbol next_symbol = symbol_after_dot(item);
                size_t next_state_id = curr_state.transitions[next_symbol];
                if (is_terminal(next_symbol)) {
                    add_shift(state_id, next_symbol, next_state_id);
                } else {
                    add_goto(state_id, next_symbol, next_state_id);
                }
            }
        }
    }
}

void TableBuilder::add_accept(size_t state_id) {
    int t_idx = terminal_index(END_MARKER);
    assert(t_idx != -1);
    action_table_[state_id][t_idx] = {ActionType::ACCEPT, 0};
}

void TableBuilder::add_reduce(size_t state_id, const FollowSet& follow_sets, Item& item) {
    const Rule& rule = GRAMMAR_RULES[item.rule_id];
    int nt_idx = nonterminal_index(rule.lhs);
    assert (nt_idx != -1);

    for (Symbol lookahead: follow_sets[nt_idx]) {
        int t_idx = terminal_index(lookahead);
        assert(t_idx != -1);
        if (action_table_[state_id][t_idx].type != ActionType::ERROR) {
            std::cerr << "SLR конфликт в состоянии " << state_id
                        << " на символе " << symbol_name(lookahead) << "\n";
        } else {
            action_table_[state_id][t_idx] = {
                ActionType::REDUCE,
                static_cast<int>(item.rule_id)
            };
        }
}

void TableBuilder::add_shift(size_t state_id, Symbol sym, size_t next_state_id) {
    int t_idx = terminal_index(sym);
    assert(t_idx != -1);
    if (action_table_[state_id][t_idx].type != ActionType::ERROR) {
        std::cerr << "SLR конфликт в состоянии " << state_id
                        << " на символе " << symbol_name(sym) << "\n";
    } else {
        action_table_[state_id][t_idx] = {ActionType::SHIFT, next_state_id};
    }
}

void TableBuilder::add_goto(size_t state_id, Symbol sym, size_t next_state_id) {
    int nt_idx = nonterminal_index(sym);
    assert(nt_idx != -1);
    goto_table_[state_id][nt_idx] = static_cast<int>(next_state_id);
}
