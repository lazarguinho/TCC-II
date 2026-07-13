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
#include <chrono>

#include "greedy.hpp"

struct GAResult
{
    std::vector<int> best_order;
    int span_value;
    std::vector<int> labeling;
    std::vector<int> span_per_generation;
    std::string stop_reason;
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
    MutationOp mutation_op,
    std::size_t max_stagnation = 150)
{
    std::mt19937 rng(std::random_device{}());
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

    int best_span_so_far = std::numeric_limits<int>::max();
    int best_ever_span   = std::numeric_limits<int>::max();
    std::vector<int> best_ever_order;
    std::vector<int> best_ever_labeling;
    std::size_t stagnant_count = 0;
    std::size_t g = 0;
    bool time_limit_hit = false;

    const auto begin      = std::chrono::steady_clock::now();
    const auto TIME_LIMIT = std::chrono::minutes(15);
    const Deadline deadline = begin + TIME_LIMIT;

    for (; g < generations; ++g)
    {
        // avalia fitness — inicializa com max para que indivíduos não avaliados
        // (interrupção por tempo) fiquem no fundo após a ordenação
        std::vector<double> fitness_values(population.size(),
                                           std::numeric_limits<double>::max());
        for (std::size_t i = 0; i < population.size(); ++i)
        {
            // passa deadline para o greedy — interrompe por vértice se o prazo vencer
            GreedyResult gr = greedy_labeling(graph, population[i], &deadline);
            if (gr.k == std::numeric_limits<int>::max())
            {
                time_limit_hit = true;
                break;
            }
            fitness_values[i] = static_cast<double>(gr.k);

            if (gr.k < best_ever_span)
            {
                best_ever_span     = gr.k;
                best_ever_order    = population[i];
                best_ever_labeling = std::move(gr.labels);
            }
        }

        // ordena pelos valores de fitness (indivíduos não avaliados ficam no fundo)
        std::vector<std::size_t> idx(population.size());
        std::iota(idx.begin(), idx.end(), 0);
        std::sort(idx.begin(), idx.end(),
                  [&](std::size_t a, std::size_t b)
                  {
                      return fitness_values[a] < fitness_values[b];
                  });

        // atualiza convergência apenas se ao menos um indivíduo foi avaliado
        if (fitness_values[idx[0]] < std::numeric_limits<double>::max())
        {
            int best_this_gen = static_cast<int>(fitness_values[idx[0]]);
            result.span_per_generation.push_back(best_this_gen);

            if (best_this_gen < best_span_so_far)
            {
                best_span_so_far = best_this_gen;
                stagnant_count   = 0;
            }
            else
            {
                ++stagnant_count;
            }
        }

        if (time_limit_hit)
            break;

        if (stagnant_count >= max_stagnation)
            break;

        if (std::chrono::steady_clock::now() - begin >= TIME_LIMIT)
        {
            time_limit_hit = true;
            break;
        }

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

    if (time_limit_hit)
        result.stop_reason = "TIME_LIMIT";
    else if (g >= generations)
        result.stop_reason = "MAX_GENS";
    else
        result.stop_reason = "STAGNATION";

    // fallback: tempo estourou antes de qualquer indivíduo ser avaliado
    // passa o deadline para não rodar indefinidamente após o prazo
    if (best_ever_order.empty())
    {
        best_ever_order = population[0];
        GreedyResult gr = greedy_labeling(graph, best_ever_order, &deadline);
        if (gr.k != std::numeric_limits<int>::max())
        {
            best_ever_span     = gr.k;
            best_ever_labeling = std::move(gr.labels);
        }
    }

    result.best_order = std::move(best_ever_order);
    result.span_value = best_ever_span;
    result.labeling   = std::move(best_ever_labeling);

    return result;
}

#endif