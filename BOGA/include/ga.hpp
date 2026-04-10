#ifndef L21_AGBO_GENETIC_ALGORITHM_HPP
#define L21_AGBO_GENETIC_ALGORITHM_HPP

#include <vector>
#include <utility>
#include <cstddef>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <stdexcept>
#include <random>
#include <functional>

#include "greedy.hpp"

struct GAResult
{
    std::vector<int> best_order;
    int span_value;
    std::vector<int> labeling;
    std::vector<int> span_per_generation;
};

std::vector<std::vector<int>>
initialize_population(std::size_t population_size, const std::vector<int> &node_ids);

std::pair<std::size_t, std::size_t>
select_parents_indices(const std::vector<double> &fitness_values);

// =======================
// Tipos de operadores injetáveis
// =======================

using CrossoverOp = std::function<
    std::pair<std::vector<int>, std::vector<int>>(const std::vector<int> &, const std::vector<int> &)>;

using MutationOp = std::function<
    std::vector<int>(
        const std::vector<int> &,
        double,
        std::mt19937 &)>;

template <typename GraphT>
inline int fitness_function(const GraphT &graph, const std::vector<int> &order)
{
    GreedyResult gr = greedy_labeling(graph, order);
    return gr.k;
}

// Algoritmo Genético principal.
// - graph: seu grafo (tipo livre, só precisa ser aceito por greedy_labeling).
// - node_ids: lista dos IDs de nós (usada para gerar as permutações iniciais).
// - population_size: tamanho da população (par).
// - generations: número de gerações.
// - mutation_rate: prob. de mutação simples por indivíduo (ex.: 0.1).
template <typename GraphT>
GAResult genetic_algorithm_labeling(
    const GraphT &graph,
    const std::vector<int> &node_ids,
    std::size_t population_size,
    std::size_t generations,
    double mutation_rate,
    double crossover_rate,
    CrossoverOp crossover_op,
    MutationOp mutation_op)
{
    std::mt19937 rng(123);
    std::uniform_real_distribution<double> dist01(0.0, 1.0);

    if (population_size % 2 != 0)
    {
        throw std::invalid_argument("population_size must be even (pares gerados em duplas).");
    }
    if (node_ids.empty())
    {
        throw std::invalid_argument("node_ids must not be empty.");
    }

    // populações
    auto population = initialize_population(population_size, node_ids);

    GAResult result;

    for (std::size_t g = 0; g < generations; ++g)
    {
        // avalia fitness (span)
        std::vector<double> fitness_values(population.size());
        for (std::size_t i = 0; i < population.size(); ++i)
        {
            fitness_values[i] = static_cast<double>(fitness_function(graph, population[i]));
        }

        // elitismo: mantém os 2 melhores
        std::vector<std::size_t> idx(population.size());
        std::iota(idx.begin(), idx.end(), 0);
        std::sort(idx.begin(), idx.end(),
                  [&](std::size_t a, std::size_t b)
                  {
                      return fitness_values[a] < fitness_values[b]; // menor é melhor
                  });

        int best_this_gen = static_cast<int>(fitness_values[idx[0]]);
        result.span_per_generation.push_back(best_this_gen);
        
        std::vector<std::vector<int>> new_population;
        new_population.reserve(population_size);
        new_population.push_back(population[idx[0]]);
        new_population.push_back(population[idx[1]]);

        // reproduz até completar
        while (new_population.size() < population_size)
        {
            auto [i, j] = select_parents_indices(fitness_values);
            const auto &p1 = population[i];
            const auto &p2 = population[j];

            std::vector<int> c1, c2;

            if (dist01(rng) < crossover_rate) {
                auto [cc1, cc2] = crossover_op(p1, p2);
                c1 = std::move(cc1);
                c2 = std::move(cc2);
            } else {
                c1 = p1;
                c2 = p2;
            }

            c1 = mutation_op(c1, mutation_rate, rng);
            c2 = mutation_op(c2, mutation_rate, rng);

            new_population.push_back(std::move(c1));
            if (new_population.size() < population_size)
            {
                new_population.push_back(std::move(c2));
            }
        }

        population = std::move(new_population);
    }

    // melhor indivíduo final
    std::vector<double> fitness_values(population.size());
    for (std::size_t i = 0; i < population.size(); ++i)
    {
        fitness_values[i] = static_cast<double>(fitness_function(graph, population[i]));
    }
    std::size_t best_idx =
        static_cast<std::size_t>(
            std::distance(fitness_values.begin(),
                          std::min_element(fitness_values.begin(), fitness_values.end())));

    auto best_order = population[best_idx];
    GreedyResult gr = greedy_labeling(graph, best_order);

    result.best_order = std::move(best_order);
    result.span_value = gr.k;
    result.labeling = std::move(gr.labels);

    return result;
}

#endif