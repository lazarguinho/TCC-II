#include "ga.hpp"

#include "selection.hpp"
#include "crossover.hpp"
#include "mutation.hpp"
#include "greedy.hpp"

#include <algorithm>
#include <numeric>
#include <random>
#include <stdexcept>

std::vector<std::vector<int>>
initialize_population(std::size_t population_size, const std::vector<int>& node_ids)
{
    if (population_size < 2) {
        throw std::invalid_argument("population_size must be >= 2");
    }
    std::vector<std::vector<int>> pop;
    pop.reserve(population_size);

    // engine por thread (determinístico se quiser seed fixa)
    static thread_local std::mt19937 rng(std::random_device{}());

    // indivíduo base: a própria ordem de node_ids
    std::vector<int> base = node_ids;

    // adiciona uma cópia não embaralhada (opcional)
    pop.push_back(base);

    // restantes: permutações aleatórias
    for (std::size_t i = 1; i < population_size; ++i) {
        std::shuffle(base.begin(), base.end(), rng);
        pop.push_back(base);
    }

    return pop;
}


std::pair<std::size_t, std::size_t>
select_parents_indices(const std::vector<double>& fitness_values)
{
    static thread_local std::mt19937 rng(std::random_device{}());
    auto [i, j] = roulette_wheel_indices(fitness_values, rng);
    return {i, j};
}