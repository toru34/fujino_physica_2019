#include <iostream>
#include "strategy.h"
#include "config.h"

Strategy::Strategy(const int p, const bool is_hetero)
{
    this->score = 0;
    if (is_hetero) {
        this->k = rng() % (p + 1);
        for (unsigned i = 0; i < p; ++i) {
            if (rng_uniform(engine) < (this->k / static_cast<float>(p))) {
                this->actions.emplace_back(OUT_ROUTE); // outside route
            } else {
                this->actions.emplace_back(IN_ROUTE); // inside route
            }
        }
    } else {
        this->k = -1;
        for (unsigned i = 0; i < p; ++i) {
            this->actions.emplace_back(rng() % 2);
        }
        for (unsigned i = 0; i < p; ++i) {
            if (rng_uniform(engine) < 0.5) {
                this->actions.emplace_back(OUT_ROUTE); // outside route
            } else {
                this->actions.emplace_back(IN_ROUTE); // inside route
            }
        }
    }
}

int Strategy::get_score()
{
    return this->score;
}

std::vector<int>& Strategy::get_score_history()
{
    return this->score_history;
}

int Strategy::choose_action(const int history_n)
{
    return this->actions.at(history_n);
}

void Strategy::update_score(const int history_n, const int winning_route)
{
    if (this->actions.at(history_n) == winning_route) {
        this->score += 1;
    } else { // actions.at(history_n) != winning_route
        this->score -= 1;
    }
}

std::ostream& operator<<(std::ostream& os, const Strategy& strategy)
{
    // heterogeneity
    os << "k:" << std::endl;
    os << "\t" << strategy.k << std::endl;
    // score
    os << "score:" << std::endl;
    os << "\t" << strategy.score << std::endl;
    // actions
    os << "actions:" << std::endl;
    for (unsigned i = 0; i < strategy.actions.size(); ++i) {
        os << "\t" << i << ": " << strategy.actions.at(i) << std::endl;
    }
    return os;
}
