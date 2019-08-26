#ifndef CONFIG_H_
#define CONFIG_H_

#include <random>
#include <vector>

extern const float ALPHA;
extern const float BETA;
extern const int N_AGENTS;
extern const int N_ITERS;
extern const int N_RUNS;
extern const int L;

extern const std::vector<int> SS;
extern const std::vector<int> MS;

extern const int LAMBDA_MIN;
extern const int LAMBDA_MAX;

extern const int INF;

// Agents' types
extern const bool IS_HETERO;
extern const bool IS_RANDOM;

// Routes
extern const int IN_ROUTE;
extern const int OUT_ROUTE;

extern const int WIN;
extern const int LOSE;

// Hub state
extern const int CONGESTED;
extern const int NON_CONGESTED;

// Random variable generator
extern std::mt19937 rng;
extern std::default_random_engine engine;
extern std::uniform_real_distribution<> rng_uniform;

// Data save option↲
extern const bool SAVE_ATTENDANCE;
extern const bool SAVE_CONGESTION;
extern const bool SAVE_WINNING;
extern const bool SAVE_ACTION;
extern const bool SAVE_COST;
extern const bool SAVE_NASH;
extern const bool SAVE_POT;

#endif
