#ifndef L21_AGBO_SELECTION_HPP
#define L21_AGBO_SELECTION_HPP

#include <vector>
#include <random>
#include <utility>
#include <stdexcept>


std::pair<std::size_t, std::size_t>
roulette_wheel_indices(const std::vector<double>& fitness_values, std::mt19937& rng);


template <typename T>
std::pair<T, T>
roulette_wheel(const std::vector<T>& population,
               const std::vector<double>& fitness_values,
               std::mt19937& rng)
{
    if (population.empty()) {
        throw std::invalid_argument("Population is empty.");
    }
    if (population.size() != fitness_values.size()) {
        throw std::invalid_argument("population and fitness_values must have the same size.");
    }

    auto [i, j] = roulette_wheel_indices(fitness_values, rng);
    return { population[i], population[j] };
}

#endif