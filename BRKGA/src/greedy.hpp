#ifndef L21_AGBO_GREEDY_HPP
#define L21_AGBO_GREEDY_HPP

#include <vector>

using Graph = std::vector<std::vector<int>>;

struct GreedyResult {
    int k;
    std::vector<int> labels;
};

GreedyResult greedy_labeling(const Graph& graph, const std::vector<int>& order);

#endif