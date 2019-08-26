#include <ctime>
#include <vector>
#include <numeric>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sys/stat.h>
#include <cstdlib>
#include "agent.h"
#include "environment.h"
#include "config.h"

template<typename T>
void save_2dvector(const std::vector<std::vector<T> >& x, const char* result_dir, const char* file_name, const int r)
{
    char path[1000];
    sprintf(path, "%s/%s_%d.csv", result_dir, file_name, r);

    std::ofstream f;
    f.open(path);
    if (!f.is_open()) {
        std::cout << "Error opening file" << std::endl;
    }
    for (auto& v : x) {
        for (auto& a : v) {
            f << a << ",";
        }
        f << "\n";
    }
    f.close();
}

template<typename T>
void save_vector(const std::vector<T>& x, const char* result_dir, const char* file_name, const int r)
{
    char path[1000];
    sprintf(path, "%s/%s_%d.csv", result_dir, file_name, r);
    std::ofstream f;
    f.open(path);
    if (!f.is_open()) {
        std::cout << "Error opening file" << std::endl;
    }
    for (auto& v : x) {
        f << v << ",";
    }
    f << "\n";
    f.close();
}

void game(Environment& environment, std::vector<Agent>& agents, const int m)
{
    // Get M recent congestion history
    std::vector<int> history = environment.get_recent_congestion_history(m);

    // Get agents' actions
    std::vector<int> actions;
    for (auto& agent : agents) {
        int action = agent.choose_action(history);
        actions.emplace_back(action);
    }

    // Determine new state
    int attendance = std::count(actions.begin(), actions.end(), IN_ROUTE);
    int new_state = environment.determine_new_state(attendance);

    // Update agents' winning history
    for (auto& agent : agents) {
        agent.update_winning_history(new_state);
    }

    // Update agents' cost history
    for (auto& agent : agents) {
        agent.update_cost_history(new_state);
    }

    // Update strategies' score
    for (auto& agent : agents) {
        agent.update_strategy_score(history, new_state);
    }
}

void run(const int n_agents, const int s, const int m, const int r, const int lambda, const int l, const float alpha, const float beta, const int n_iters, const bool is_hetero, const bool is_random, const char* result_dir)
{
    // Initialize environment
    Environment environment(n_agents, lambda, l, alpha, beta);


    // Initialize agents
    std::vector<Agent> agents;
    for (unsigned i = 0; i < n_agents; ++i) {
        int destination = rng() % (n_agents - 1);
        if (i == destination) {
            destination = n_agents - 1;
        }
        float cost_out = environment.compute_cost_out(i, destination);
        std::vector<float> costs_in = environment.compute_costs_in(i, destination);
        float cost_in_con = costs_in.at(0);
        float cost_in_ncon = costs_in.at(1);
        agents.emplace_back(Agent(s, m, i, destination, cost_out, cost_in_con, cost_in_ncon, is_hetero, is_random));
    }

    // Compute nash equilibrium
    std::vector<std::vector<float> > costs;
    for (auto& agent : agents) {
        float cost_out = agent.get_cost_out();
        float cost_in_ncon = agent.get_cost_in_ncon();
        float cost_diff = cost_out - cost_in_ncon;
        std::vector<float> cost = {cost_out, cost_in_ncon, cost_diff};

        costs.emplace_back(cost);
    }

    std::sort(costs.begin(), costs.end(), [](const std::vector<float>& a, const std::vector<float>& b){ return a[2] > b[2];});

    float total_cost = 0;
    for (unsigned i = 0; i < N_AGENTS; ++i) {
        if (i < L && costs.at(i).at(2) > 0) {
            total_cost += costs.at(i).at(1);
        } else {
            total_cost += costs.at(i).at(0);
        }
    }
    float nash_best = total_cost / N_AGENTS;

    std::sort(costs.begin(), costs.end(), [](const std::vector<float>& a, const std::vector<float>& b){ return a[2] < b[2];});

    total_cost = 0;
    int n_in = 0;
    for (unsigned i = 0; i < N_AGENTS; ++i) {
        if (costs.at(i).at(2) < 0) {
            total_cost += costs.at(i).at(0);
        } else if (n_in < L) {
            total_cost += costs.at(i).at(1);
            ++n_in;
        } else {
            total_cost += costs.at(i).at(0);
        }
    }
    float nash_worst = total_cost / N_AGENTS;

    // Iterate games
    for (unsigned i = 0; i < n_iters; ++i) {
        game(environment, agents, m);
    }

    // Save attendance
    if (SAVE_ATTENDANCE) {
        std::vector<int> attendance_history = environment.get_attendance_history();
        save_vector(attendance_history, result_dir, "attendance", r);
    }

    // Save congestion
    if (SAVE_CONGESTION) {
        std::vector<int> congestion_history = environment.get_congestion_history();
        save_vector(congestion_history, result_dir, "congestion", r);
    }

    // Save winning
    if (SAVE_WINNING) {
        std::vector<std::vector<int> > winning_histories;
        for (auto& agent : agents) {
            winning_histories.emplace_back(agent.get_winning_history());
        }
        save_2dvector(winning_histories, result_dir, "winning", r);
    }

    // Save actions
    if (SAVE_ACTION) {
        std::vector<std::vector<int> > action_histories;
        for (auto& agent : agents) {
            action_histories.emplace_back(agent.get_action_history());
        }
        save_2dvector(action_histories, result_dir, "action", r);
    }

    // Save costs
    if (SAVE_COST) {
        std::vector<std::vector<float> > cost_histories;
        for (auto& agent : agents) {
            cost_histories.emplace_back(agent.get_cost_history());
        }
        save_2dvector(cost_histories, result_dir, "cost", r);
    }

    // Save nash equilibrium
    if (SAVE_NASH) {
        std::vector<float> nash = {nash_best, nash_worst};
        save_vector(nash, result_dir, "nash", r);
    }

    // Save potentiality
    if (SAVE_POT) {
        std::vector<int> pots;
        for (auto& agent : agents) {
            pots.emplace_back(static_cast<int>(agent.get_potentiality()));
        }
        save_vector(pots, result_dir, "pot", r);
    }
}

int main()
{
    std::clock_t start, end;
    start = std::clock();

    std::vector<int> lambdas;
    for (unsigned i = LAMBDA_MIN; i <= LAMBDA_MAX; ++i) {
        lambdas.emplace_back(i);
    }

    for (auto m : MS) {
        for (auto s : SS) {
            for (auto lambda : lambdas) {
                // Create result directory
                char result_dir[1000];
                sprintf(result_dir, "../result/m%d_s%d_nagents%d_niters%d_nruns%d_l%d_lambda%d_hetero%d_random%d", m, s, N_AGENTS, N_ITERS, N_RUNS, L, lambda, IS_HETERO, IS_RANDOM);

                char command[1000];
                sprintf(command, "mkdir -p %s", result_dir);
                system(command);
                // Run experiment
                for (int r = 0; r < N_RUNS; ++r) {
                    run(N_AGENTS, s, m, r, lambda, L, ALPHA, BETA, N_ITERS, IS_HETERO, IS_RANDOM, result_dir);
                }

                end = std::clock();
                std::cout << "S: " << s << ", M: " << m << ", Lambda: " << lambda << ", Time: " << (end - start) / 1000000 << " [s]" << std::endl;
            }
        }
    }


    return 0;
}
