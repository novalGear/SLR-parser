#include "table_builder.hpp"
#include "symbol_utils.hpp"
#include "rule.hpp"
#include "dbg_utils.hpp"

#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <cassert>
#include <queue>
#include <string>

void TableBuilder::build(const FollowSet& follow_sets) {
    build_states();
    DBG_PRINT("states built\n");
    dump_states();
    dump_states_dot("./data/states.dot");
    dump_states_json("./data/states.json");
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
        size_t state_id = q.front(); q.pop();
        std::set<Symbol> successor_symbols = get_next_symbols(states_[state_id].items);

        for (Symbol symbol : successor_symbols) {
            // DBG_PRINT("\n\ncurrent state print in for cycle (symbol = %s):\n",
            //             symbol_name(symbol));
            // dump_state(states_[state_id]);
            // DBG_PRINT("\n\ndump states_[0]");
            // dump_state(states_[0]);

            ItemSet goto_items_res = goto_items(states_[state_id].items, symbol);
            // DBG_PRINT("\ngoto items (%d, %s):\n", static_cast<int>(symbol), symbol_name(symbol));
            // for (const Item& item : goto_items_res) {
            //     print_item(item);
            // }

            ItemSet target_items = closure(goto_items_res);
            // DBG_PRINT("\nclosure:\n");
            // for (const Item& item : target_items) {
            //     print_item(item);
            // }

            auto it = state_to_id_.find(target_items);
            if (it == state_to_id_.end()) {
                // new state
                size_t new_state_id = states_.size();
                states_.push_back({target_items, {}});
                state_to_id_[target_items] = new_state_id;
                states_[state_id].transitions[symbol] = new_state_id;
                q.push(new_state_id);

                // DBG_PRINT("State %ld ==========\n", new_state_id);
                // for (const Item& item : target_items) {
                //     print_item(item);
                // }


            } else {
                // уже существующее состояние
                states_[state_id].transitions[symbol] = it->second;
            }
            // DBG_PRINT("current state (%ld) transitions:\n", state_id);
            // dump_state(states_[state_id]);
        }
    }
}

void TableBuilder::fill_tables(const FollowSet& follow_sets) {
    // init actions, goto tables
    size_t states_count = states_.size();
    action_table_.assign(states_count, std::vector<Action>(TERMINAL_COUNT, {ActionType::ERROR, -1}));
    goto_table_.assign(states_count, std::vector<int>(NONTERMINAL_COUNT, -1));

    for (size_t state_id = 0; state_id < states_count; ++state_id) {
        for (const Item& item : states_[state_id].items) {
            if (is_complete(item)) {
                const Rule& rule = GRAMMAR_RULES[item.rule_id];
                if (rule.lhs == START_SYMBOL) {
                    add_accept(state_id);
                } else {
                    add_reduce(state_id, follow_sets, item);
                }
            } else {
                Symbol next_symbol = symbol_after_dot(item);
                DBG_PRINT("next_symbol: %d", static_cast<int>(next_symbol));
                size_t next_state_id = states_[state_id].transitions.at(next_symbol);
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

void TableBuilder::add_reduce(size_t state_id, const FollowSet& follow_sets, const Item& item) {
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
}

void TableBuilder::add_shift(size_t state_id, Symbol sym, size_t next_state_id) {
    int t_idx = terminal_index(sym);
    assert(t_idx != -1);
    if (action_table_[state_id][t_idx].type != ActionType::ERROR) {
        std::cerr << "SLR конфликт в состоянии " << state_id
                        << " на символе " << symbol_name(sym) << "\n";
    } else {
        action_table_[state_id][t_idx] = {ActionType::SHIFT, static_cast<int>(next_state_id)};
    }
}

void TableBuilder::add_goto(size_t state_id, Symbol sym, size_t next_state_id) {
    int nt_idx = nonterminal_index(sym);
    assert(nt_idx != -1);
    goto_table_[state_id][nt_idx] = static_cast<int>(next_state_id);
}

void dump_state(const State& state) {
    // Печать всех пунктов
    for (const Item& item : state.items) {
        print_item(item);
    }

    // Печать переходов
    if (!state.transitions.empty()) {
        std::cout << "Transitions:\n";
        for (const auto& [symbol, target] : state.transitions) {
            std::cout << "  " << symbol_name(symbol) << " -> " << target << "\n";
        }
    }
}

void TableBuilder::dump_states() const {
    for (size_t i = 0; i < states_.size(); ++i) {
        std::cout << "\n=== State " << i << " ===\n";
        dump_state(states_[i]);
    }
}
// В table_builder.cpp
void TableBuilder::dump_states_dot(const std::string& filename) const {
    std::ofstream file(filename);
    file << "digraph LR_Automaton {\n";
    file << "    rankdir=LR;\n";
    file << "    splines=ortho;\n";
    file << "    nodesep=0.6;\n";
    file << "    ranksep=1.2;\n";
    file << "    node [shape=box, style=filled, fillcolor=\"#f0f0f0\", fontname=\"Courier\"];\n";
    file << "    edge [fontname=\"Courier\", fontsize=10];\n";

    // Состояния
    for (size_t i = 0; i < states_.size(); ++i) {
        std::string label = "\"" + std::to_string(i) + "\\l";
        for (const Item& item : states_[i].items) {
            label += format_item(item) + "\\l";
        }
        label += "\"";
        file << "    " << i << " [label=" << label << "];\n";
    }

    // Переходы
    for (size_t i = 0; i < states_.size(); ++i) {
        for (const auto& [sym, target] : states_[i].transitions) {
            std::string color = is_terminal(sym) ? "blue" : "black";
            file << "    " << i << " -> " << target
                 << " [label=\"" << symbol_name(sym)
                 << "\", color=\"" << color << "\"];\n";
        }
    }
    file << "}\n";
}
void TableBuilder::dump_states_json(const std::string& filename) const {
    nlohmann::json j;
    j["nodes"] = nlohmann::json::array();
    j["links"] = nlohmann::json::array();

    // Состояния
    for (size_t i = 0; i < states_.size(); ++i) {
        // Собираем все пункты в одну строку с \n
        std::string label;
        for (const Item& item : states_[i].items) {
            label += format_item(item) + "\n";
        }
        if (!label.empty()) {
            label.pop_back(); // удаляем последний \n
        }

        nlohmann::json node;
        node["id"] = std::to_string(i);      // ← строка!
        node["label"] = label;               // ← одна строка
        j["nodes"].push_back(node);
    }

    // Рёбра
    for (size_t i = 0; i < states_.size(); ++i) {
        for (const auto& [sym, target] : states_[i].transitions) {
            nlohmann::json link;
            link["source"] = std::to_string(i);        // ← строка!
            link["target"] = std::to_string(target);   // ← строка!
            link["label"] = symbol_name(sym);
            j["links"].push_back(link);
        }
    }

    std::ofstream file(filename);
    file << j.dump(2);
}
