#ifndef AGENT_H_
#define AGENT_H_

#include <vector>
#include <iostream>
#include "strategy.h"

class Agent {
    int node_n;
    int destination;
    bool potential;
    bool is_hetero;
    bool is_random;
    float cost_out;
    float cost_in_con;
    float cost_in_ncon;
    std::vector<int> action_history;
    std::vector<int> winning_history;
    std::vector<int> strategy_history;
    std::vector<float> cost_history;
    std::vector<Strategy> strategies;
public:
    Agent(const int s, const int m, const int node_n, const int destination, const float cost_out, const float cost_in_con, const float cost_in_ncon, const bool is_hetero, const bool is_random);
    int get_best_strategy_n();
    int choose_action(const std::vector<int> history);
    int get_winning_route(const int new_state);
    bool get_potentiality();
    float get_cost_out();
    float get_cost_in_ncon();
    std::vector<int>& get_action_history();
    std::vector<int>& get_winning_history();
    std::vector<float>& get_cost_history();
    void update_strategy_score(const std::vector<int>& history, const int new_state);
    void update_cost_history(const int new_state);
    void update_winning_history(const int new_state);
    std::vector<int> get_individual_history(const std::vector<int> history);
    friend std::ostream& operator<<(std::ostream& os, const Agent& agent);
};

#endif
