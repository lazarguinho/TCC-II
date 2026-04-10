#ifndef L21_AGBO_MUTATION_HPP
#define L21_AGBO_MUTATION_HPP

#include <vector>
#include <cstddef>
#include <random>

/**
 * Troca dois genes escolhidos aleatoriamente.
 * @param individual Permutação de entrada.
 * @param mutation_rate Probabilidade de aplicar a mutação (0..1).
 * @return Nova permutação possivelmente mutada.
 * @throws std::invalid_argument se mutation_rate não estiver em [0,1].
 */
std::vector<int> exchange_mutation(const std::vector<int>& individual, double mutation_rate, std::mt19937& rng);

/**
 * Seleciona um segmento contínuo e o desloca para outra posição,
 * preservando a ordem relativa dos demais genes.
 */
std::vector<int> displacement_mutation(const std::vector<int>& individual, double mutation_rate, std::mt19937& rng);

/**
 * Seleciona um segmento contínuo, inverte sua ordem e reinsere
 * o segmento invertido em uma posição aleatória.
 */
std::vector<int> inversion_mutation(const std::vector<int>& individual, double mutation_rate, std::mt19937& rng);

/**
 * Seleciona um segmento contínuo e embaralha aleatoriamente
 * os elementos apenas dentro desse intervalo.
 */
std::vector<int> scramble_mutation(const std::vector<int>& individual, double mutation_rate, std::mt19937& rng);

/**
 * Remove um único gene e o insere em uma nova posição,
 * deslocando os demais para manter a sequência.
 */
std::vector<int> insertion_mutation(const std::vector<int>& individual, double mutation_rate, std::mt19937& rng);

/**
 * Seleciona dois pontos de corte (l < r) e inverte o segmento [l, r] in-place.
 */
std::vector<int> simple_inversion_mutation(const std::vector<int>& individual, double mutation_rate, std::mt19937& rng);

#endif // L21_AGBO_MUTATION_HPP