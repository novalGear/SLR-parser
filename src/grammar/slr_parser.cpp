#include "slr_parser.hpp"
#include "first_follow.hpp"

int SLR_Parser::build_tables() {
    FirstSet first = create_first_set();
    FollowSet follow = create_follow_set(first);
    print_first_follow(first, follow);

    std::map<ItemSet, size_t> states_to_id = build_states();
    fill_tables(states_to_id);
}

std::set<Symbol> get_next_symbols(const ItemSet& item_set) {
    std::set<Symbol> result;
    for (const Item& item : item_set) {
        if (!is_complete(item)) {
            result.insert(symbol_after_dot(item));
        }
    }
    return result;
}

SLR_Parser::build_states() {

    std::vector<ItemSet> states;
    std::<ItemSet, size_t> states_to_id;

    ItemSet initial = initial_set();
    states.push_back(initial);
    state_to_id[initial] = 0;

    std::queue<size_t> q;
    q.push(0);

    // добавляем все получаемые состояния в очередь,
    // на каждой итерации while работаем с одним состоянием

    while(!q.empty()) {
        size_t state_id = q.front(); q.pop();
        const State& I = states[state_id];
        std::set<Symbol> goto_terminals = get_next_symbols(state);

        for (Symbol sym : goto_terminals) {

            ItemSet new_set = closure(goto_items(state.items, sym));

            if (auto it = state_to_id.find(new_set); it == states_to_id.end()) {
                size_t new_set_id = states.size();
                states.push_back(new_set);
                states_to_id[new_set] = new_set_id;
                q.push();
            }


        }
    }

    return states_to_id;
}

int SLR_Parser(std::map<ItemSet, size_t>& states_to_id) {

}
