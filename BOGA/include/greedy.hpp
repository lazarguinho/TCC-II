#ifndef L21_AGBO_GREEDY_HPP
#define L21_AGBO_GREEDY_HPP

#include <vector>
#include <utility>
#include <chrono>
#include <limits>

using Graph = std::vector<std::vector<int>>;
using Deadline = std::chrono::steady_clock::time_point;

struct GreedyResult {
    int k;
    std::vector<int> labels;
};

// Se deadline != nullptr e o prazo for atingido durante o loop de vértices,
// retorna {INT_MAX, {}} como sentinela de interrupção.
GreedyResult greedy_labeling(const Graph& graph, const std::vector<int>& order,
                              const Deadline* deadline = nullptr);

#endif
