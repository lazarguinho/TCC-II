#ifndef L21_AGBO_CROSSOVER_HPP
#define L21_AGBO_CROSSOVER_HPP

#include <vector>
#include <utility>
#include <cstddef>

/**
 * Cycle Crossover (CX)
 *
 * Identifica ciclos de posições entre os dois pais e preserva os genes
 * do primeiro pai nessas posições; os demais índices recebem genes do segundo pai.
 *
 * @throws std::invalid_argument se tamanhos diferirem ou não forem permutações compatíveis.
 */
std::pair<std::vector<int>, std::vector<int>>
cycle_crossover(const std::vector<int>& p1, const std::vector<int>& p2);

/**
 * Order Crossover (OX)
 *
 * Seleciona um segmento contínuo de um dos pais e preserva esse bloco na mesma posição.
 * Os demais genes são preenchidos respeitando a ordem relativa do outro pai (preenchimento circular).
 *
 * @throws std::invalid_argument se tamanhos diferirem ou não forem permutações compatíveis.
 */
std::pair<std::vector<int>, std::vector<int>>
order_crossover(const std::vector<int>& p1, const std::vector<int>& p2);


#endif // L21_AGBO_CROSSOVER_HPP