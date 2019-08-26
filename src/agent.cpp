#include <cmath>
#include <algorithm>
#include "utils.h"
#include "agent.h"
#include "config.h"

Agent::Agent(const int s, const int m, const int node_n, const int destination, const float cost_out, const float cost_in_con, const float cost_in_ncon, const bool is_hetero, const bool is_random)
{
    int p = std::pow(2, m); // number of histories
    for (unsigned i = 0; i < s; ++i) {
        this->strategies.emplace_back(Strategy(p, is_hetero));
    }

    this->is_hetero = is_hetero;
    this->is_random = is_random;
    this->node_n = node_n;
    this->destination = destination;
    this->cost_out = cost_out;
    this->cost_in_con = cost_in_con;
    this->cost_in_ncon = cost_in_ncon;
    if (this->cost_in_ncon < this->cost_out) {
        this->potential = true;
    } else {
        this->potential = false;
    }
}

bool Agent::get_potentiality()
{
    return this->potential;
}

float Agent::get_cost_out()
{
    return this->cost_out;
}

float Agent::get_cost_in_ncon()
{
    return this->cost_in_ncon;
}

std::vector<int>& Agent::get_action_history()
{
    return this->action_history;
}

std::vector<int>& Agent::get_winning_history()
{
    return this->winning_history;
}

std::vector<float>& Agent::get_cost_history()
{
    return this->cost_history;
}

int Agent::get_best_strategy_n()
{
    int best_score = -INF;

    // Get best score
    for (unsigned i = 0; i < this->strategies.size(); ++i) {
        if (strategies.at(i).get_score() > best_score) {
            best_score = strategies.at(i).get_score();
        }
    }

    // Get best strategy numbers
    std::vector<int> best_strategy_ns;
    for (unsigned i = 0; i < this->strategies.size(); ++i) {
        if (strategies.at(i).get_score() == best_score) {
            best_strategy_ns.emplace_back(i);
        }
    }

    // Get best strategy number
    int best_strategy_n = best_strategy_ns.at(rng() % best_strategy_ns.size());

    return best_strategy_n;
}

int Agent::get_winning_route(const int new_state)
{
    if (new_state == CONGESTED) {
        if (this->cost_in_con < this->cost_out) {
            return IN_ROUTE;
        } else if (this->cost_out < this->cost_in_con) {
            return OUT_ROUTE;
        } else { // cost_out == cost_in_con
            return OUT_ROUTE;
        }
    } else { // new_state == NON_CONGESTED
        if (this->cost_in_ncon < this->cost_out) {
            return IN_ROUTE;
        } else if (this->cost_out < this->cost_in_ncon) {
            return OUT_ROUTE;
        } else { // cost_out == cost_in_ncon
            return OUT_ROUTE;
        }
    }
}

void Agent::update_strategy_score(const std::vector<int>& history, const int new_state)
{
    const std::vector<int> individual_history = this->get_individual_history(history);
    const int history_n = utils::get_history_n(individual_history);
    const int winning_route = this->get_winning_route(new_state);

    for (auto& strategy : this->strategies) {
        strategy.update_score(history_n, winning_route);
    }
}

void Agent::update_cost_history(const int new_state)
{
    const int action = this->action_history.back(); // Get current action
    if (action == IN_ROUTE) {
        if (new_state == CONGESTED) {
            this->cost_history.emplace_back(this->cost_in_con);
        } else { // new_state == NON_CONGESTED
            this->cost_history.emplace_back(this->cost_in_ncon);
        }
    } else { // action == OUT_ROUTE
        this->cost_history.emplace_back(this->cost_out);
    }
}

void Agent::update_winning_history(const int new_state)
{
    const int action = this->action_history.back(); // Get current action
    if (action == IN_ROUTE) {
        if (new_state == CONGESTED) {
            if (this->cost_in_con < this->cost_out) {
                this->winning_history.emplace_back(WIN);
            } else if (this->cost_out < this->cost_in_con) {
                this->winning_history.emplace_back(LOSE);
            } else { // cost_out == cost_in_con
                this->winning_history.emplace_back(LOSE);
            }
        } else { // new_state == NON_CONGESTED
            if (this->cost_in_ncon < this->cost_out) {
                this->winning_history.emplace_back(WIN);
            } else if (this->cost_out < this->cost_in_ncon) {
                this->winning_history.emplace_back(LOSE);
            } else { // cost_out == cost_in_ncon
                this->winning_history.emplace_back(LOSE);
            }
        }
    } else { // action == OUT_ROUTE
        if (new_state == CONGESTED) {
            if (this->cost_in_con < this->cost_out) {
                this->winning_history.emplace_back(LOSE);
            } else if (this->cost_out < this->cost_in_con) {
                this->winning_history.emplace_back(WIN);
            } else { // cost_out == cost_in_con
                this->winning_history.emplace_back(WIN);
            }
        } else { // new_state == NON_CONGESTED
            if (this->cost_in_ncon < this->cost_out) {
                this->winning_history.emplace_back(LOSE);
            } else if (this->cost_out < this->cost_in_ncon) {
                this->winning_history.emplace_back(WIN);
            } else { // cost_out == cost_in_con
                this->winning_history.emplace_back(WIN);
            }
        }
    }
}

int Agent::choose_action(const std::vector<int> history)
{
    int action;
    if (this->is_random) {
        if (rng_uniform(engine) < 0.5) {
            action = OUT_ROUTE;
        } else {
            action = IN_ROUTE;
        }
        this->action_history.emplace_back(action);
    } else {
        const int best_strategy_n = this->get_best_strategy_n();
        Strategy& best_strategy = this->strategies.at(best_strategy_n);
        const std::vector<int> individual_history = this->get_individual_history(history);
        const int history_n = utils::get_history_n(individual_history);

        action = best_strategy.choose_action(history_n);
        this->action_history.emplace_back(action);
        this->strategy_history.emplace_back(best_strategy_n);
    }
    return action;
}

std::vector<int> Agent::get_individual_history(const std::vector<int> history)
{
    std::vector<int> individual_history;
    if (this->cost_in_ncon < this->cost_out) {
        individual_history = history;
    } else if (this->cost_out < this->cost_in_ncon) {
        individual_history.resize(history.size(), OUT_ROUTE);
    } else { // cost_out == cost_in_ncon
        individual_history.resize(history.size(), OUT_ROUTE);
    }

    return individual_history;
}

std::ostream& operator<<(std::ostream& os, const Agent& agent)
{
    os << "origin (node_n):" << std::endl;
    os << "\t" << agent.node_n << std::endl;
    os << "destination:" << std::endl;
    os << "\t" << agent.destination << std::endl;
    os << "cost_out:" << std::endl;
    os << "\t" << agent.cost_out << std::endl;
    os << "cost_in_con:" << std::endl;
    os << "\t" << agent.cost_in_con << std::endl;
    os << "cost_in_ncon:" << std::endl;
    os << "\t" << agent.cost_in_ncon << std::endl;

    return os;
}
