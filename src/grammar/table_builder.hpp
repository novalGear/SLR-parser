#pragma once

#include "first_follow.hpp"
#include "slr_parser.hpp"

class TableBuilder {
public:
    void build(const FollowSet& follow_sets);
    // Доступ к результатам (rvalue reference)
    std::vector<State>&& take_states()    { return std::move(states_); }
    ActionTable&& take_action_table()     { return std::move(action_table_); }
    GotoTable&& take_goto_table()         { return std::move(goto_table_); }

private:
    std::vector<State> states_;                 // хранилище состояний
    std::map<ItemSet, size_t> state_to_id_;     // для выявления дубликатов
    ActionTable action_table_;
    GotoTable goto_table_;

    // Основные этапы
    void build_states();
    void fill_tables(const FollowSet& follow_sets);

    // вспомогательные методы для fill_tables
    void add_accept(size_t state_id);
    void add_reduce(size_t state_id, const FollowSet& follow_sets, const Item& item);
    void add_shift (size_t state_id, Symbol sym, size_t next_state_id);
    void add_goto  (size_t state_id, Symbol sym, size_t next_state_id);

    // отладка
    void dump_states() const;
};

void dump_state(const State& state);
