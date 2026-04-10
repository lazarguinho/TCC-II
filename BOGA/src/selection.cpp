#include "selection.hpp"

#include <numeric>
#include <limits>
#include <algorithm>
#include <random>

std::pair<std::size_t, std::size_t>
roulette_wheel_indices(const std::vector<double>& fitness_values, std::mt19937& rng)
{
    if (fitness_values.empty()) {
        throw std::invalid_argument("fitness_values is empty.");
    }

    // Soma total do fitness
    const double total_fitness = std::accumulate(fitness_values.begin(),
                                                 fitness_values.end(), 0.0);

    // Probabilidades inversas: p_i ∝ 1 - f_i / total_fitness
    // (quanto menor f_i, maior o peso).
    std::vector<double> inverse_probs;
    inverse_probs.reserve(fitness_values.size());
    for (double f : fitness_values) {
        double w = 1.0 - (f / total_fitness);
        // se por numéricos ocorrer w<0 (fitness muito grande), trunca em 0
        if (w < 0.0) w = 0.0;
        inverse_probs.push_back(w);
    }

    // Se todos pesos ficaram 0 (ex.: todos fitness iguais e muito grandes),
    // caímos para distribuição uniforme.
    const bool all_zero = std::all_of(inverse_probs.begin(), inverse_probs.end(),
                                      [](double w){ return w <= 0.0; });

    if (all_zero) {
        std::vector<double> uniform(fitness_values.size(), 1.0);
        std::discrete_distribution<std::size_t> dist(uniform.begin(), uniform.end());
        return { dist(rng), dist(rng) };
    }

    std::discrete_distribution<std::size_t> dist(inverse_probs.begin(), inverse_probs.end());
    std::size_t p1 = dist(rng);
    std::size_t p2 = dist(rng);
    return { p1, p2 };
}