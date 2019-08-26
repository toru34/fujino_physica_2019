#ifndef STRATEGY_H_
#define STRATEGY_H_

#include <vector>

class Strategy {
    int k;
    int score;
    std::vector<int> actions;
    std::vector<int> score_history;
public:
    Strategy(const int p, const bool is_hetero);
    int get_score();
    std::vector<int>& get_score_history();
    int choose_action(const int history_n);
    void update_score(const int history_n, const int winning_route);
    friend std::ostream& operator<<(std::ostream& os, const Strategy& strategy);
};

#endif
