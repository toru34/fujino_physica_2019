#ifndef UTILS_H_
#define UTILS_H_

#include <cmath>
#include <vector>

namespace utils {

int get_history_n(const std::vector<int>& history)
{
    int history_n = 0;
    int m = history.size();
    for (unsigned i = 0; i < m; ++i) {
        int power = std::pow(2, m - i - 1);
        history_n += power * history.at(i);
    }
    return history_n;
}

} // namespace utils

#endif
