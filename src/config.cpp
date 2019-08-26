#include "config.h"

const float ALPHA = 0.5;
const float BETA = 1.5;
const int N_AGENTS = 100;
const int N_ITERS = 1000;
const int N_RUNS = 1000;
const int L = 100;

const std::vector<int> SS = {8};
const std::vector<int> MS = {2};

const int LAMBDA_MIN = 2;
const int LAMBDA_MAX = N_AGENTS;

const int INF = 1001001001;

// Agents' types
const bool IS_HETERO = true;
const bool IS_RANDOM = false;

// Routes
const int IN_ROUTE = 1;
const int OUT_ROUTE = 0;

const int WIN = 1;
const int LOSE = 0;

// Hub state
const int CONGESTED = 1;
const int NON_CONGESTED = 0;

// Random variable generator
std::mt19937 rng(34);
std::default_random_engine engine(34);
std::uniform_real_distribution<> rng_uniform(0.0, 1.0);

// Data save option
const bool SAVE_ATTENDANCE = false;
const bool SAVE_CONGESTION = false;
const bool SAVE_WINNING = false;
const bool SAVE_ACTION = false;
const bool SAVE_COST = true;
const bool SAVE_NASH = false;
const bool SAVE_POT = false;
