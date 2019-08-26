#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_

#include <vector>

class Environment {
    int n; // network size
    int l; // hub capacity
    float alpha; // uncongestion parameter
    float beta; // congestion parameter
    std::vector<int> congestion_history;
    std::vector<int> attendance_history;
    std::vector<bool> hub_links;
public:
    Environment(const int n, const int lmd, const int l, const float alpha, const float beta);
    float compute_cost_out(const int origin, const int destination);
    std::vector<float> compute_costs_in(const int origin, const int destination);
    void update_congestion_history(const int new_state);
    void update_attendance_history(const int attendance);
    int determine_new_state(const int attendance);
    std::vector<int> get_recent_congestion_history(const int m);
    std::vector<int>& get_attendance_history();
    std::vector<int>& get_congestion_history();
    friend std::ostream& operator<<(std::ostream& os, const Environment& environment);
};

#endif
