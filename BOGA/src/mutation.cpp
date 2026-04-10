#include "mutation.hpp"
#include <random>
#include <stdexcept>

std::vector<int> exchange_mutation(const std::vector<int>& individual, double mutation_rate, std::mt19937& rng) {
    if (mutation_rate < 0.0 || mutation_rate > 1.0) {
        throw std::invalid_argument("mutation_rate must be between 0 and 1.");
    }

    std::vector<int> mutated = individual;

    std::uniform_real_distribution<double> dist01(0.0, 1.0);

    if (dist01(rng) < mutation_rate && mutated.size() >= 2) {
        std::uniform_int_distribution<std::size_t> distIndex(0, mutated.size() - 1);
        std::size_t i = distIndex(rng);
        std::size_t j = distIndex(rng);
        while (j == i) {
            j = distIndex(rng); // garante índices distintos
        }
        std::swap(mutated[i], mutated[j]);
    }

    return mutated;
}

std::vector<int> displacement_mutation(const std::vector<int>& individual, double mutation_rate, std::mt19937& rng) {
    if (mutation_rate < 0.0 || mutation_rate > 1.0) {
        throw std::invalid_argument("mutation_rate must be between 0 and 1.");
    }

    std::vector<int> mutated = individual;

    std::uniform_real_distribution<double> dist01(0.0, 1.0);

    if (dist01(rng) < mutation_rate && mutated.size() >= 2) {
        const std::size_t n = mutated.size();

        // Escolhe um segmento contínuo [l, r] (não vazio)
        std::uniform_int_distribution<std::size_t> distL(0, n - 1);
        std::size_t l = distL(rng);

        std::uniform_int_distribution<std::size_t> distR(l, n - 1); // garante r >= l
        std::size_t r = distR(rng);

        // Extrai o segmento
        const std::size_t seg_len = r - l + 1;
        std::vector<int> segment(mutated.begin() + static_cast<std::ptrdiff_t>(l),
                                 mutated.begin() + static_cast<std::ptrdiff_t>(r) + 1);

        // Remove o segmento do indivíduo (mantém ordem relativa dos demais)
        mutated.erase(mutated.begin() + static_cast<std::ptrdiff_t>(l),
                      mutated.begin() + static_cast<std::ptrdiff_t>(r) + 1);

        // Escolhe nova posição de inserção no vetor reduzido (0..remaining_size)
        // Inserção em 'pos' é antes do elemento atualmente em 'pos'.
        std::uniform_int_distribution<std::size_t> distPos(0, mutated.size());
        std::size_t pos = distPos(rng);

        // Insere o segmento na nova posição (preserva ordem interna do segmento)
        mutated.insert(mutated.begin() + static_cast<std::ptrdiff_t>(pos),
                       segment.begin(), segment.end());
        // Obs.: se o segmento cobrir tudo ou se 'pos' coincidir com a posição original
        // após remoção, o efeito pode ser um no-op, o que é aceitável em mutações probabilísticas.
    }

    return mutated;
}

std::vector<int> inversion_mutation(const std::vector<int>& individual, double mutation_rate, std::mt19937& rng) {
    if (mutation_rate < 0.0 || mutation_rate > 1.0) {
        throw std::invalid_argument("mutation_rate must be between 0 and 1.");
    }

    std::vector<int> mutated = individual;

    std::uniform_real_distribution<double> dist01(0.0, 1.0);

    if (dist01(rng) < mutation_rate && mutated.size() >= 2) {
        const std::size_t n = mutated.size();

        // Sorteia um segmento contínuo [l, r] (não vazio)
        std::uniform_int_distribution<std::size_t> distL(0, n - 1);
        std::size_t l = distL(rng);
        std::uniform_int_distribution<std::size_t> distR(l, n - 1);
        std::size_t r = distR(rng);

        // Extrai o segmento
        std::vector<int> segment(mutated.begin() + static_cast<std::ptrdiff_t>(l),
                                 mutated.begin() + static_cast<std::ptrdiff_t>(r) + 1);

        // Inverte a ordem dos elementos do segmento
        std::reverse(segment.begin(), segment.end());

        // Remove o segmento do indivíduo original (mantém ordem dos demais genes)
        mutated.erase(mutated.begin() + static_cast<std::ptrdiff_t>(l),
                      mutated.begin() + static_cast<std::ptrdiff_t>(r) + 1);

        // Escolhe nova posição de inserção no vetor reduzido (0..mutated.size())
        // Inserção em 'pos' ocorre antes do elemento atualmente em 'pos'.
        std::uniform_int_distribution<std::size_t> distPos(0, mutated.size());
        std::size_t pos = distPos(rng);

        // Insere o segmento invertido na nova posição
        mutated.insert(mutated.begin() + static_cast<std::ptrdiff_t>(pos),
                       segment.begin(), segment.end());
        // Observação: se o segmento cobrir todo o indivíduo e 'pos' for 0,
        // o efeito é só inverter tudo — o que é válido para este operador.
    }

    return mutated;
}

std::vector<int> scramble_mutation(const std::vector<int>& individual, double mutation_rate, std::mt19937& rng) {
    if (mutation_rate < 0.0 || mutation_rate > 1.0) {
        throw std::invalid_argument("mutation_rate must be between 0 and 1.");
    }

    std::vector<int> mutated = individual;

    std::uniform_real_distribution<double> dist01(0.0, 1.0);

    if (dist01(rng) < mutation_rate && mutated.size() >= 2) {
        const std::size_t n = mutated.size();

        // Seleciona um segmento contínuo [l, r] (não vazio)
        std::uniform_int_distribution<std::size_t> distL(0, n - 1);
        std::size_t l = distL(rng);
        std::uniform_int_distribution<std::size_t> distR(l, n - 1);
        std::size_t r = distR(rng);

        // Embaralha o segmento selecionado
        std::shuffle(mutated.begin() + static_cast<std::ptrdiff_t>(l),
                     mutated.begin() + static_cast<std::ptrdiff_t>(r) + 1,
                     rng);
    }

    return mutated;
}

std::vector<int> insertion_mutation(const std::vector<int>& individual, double mutation_rate, std::mt19937& rng) {
    if (mutation_rate < 0.0 || mutation_rate > 1.0) {
        throw std::invalid_argument("mutation_rate must be between 0 and 1.");
    }

    std::vector<int> mutated = individual;

    std::uniform_real_distribution<double> dist01(0.0, 1.0);

    if (dist01(rng) < mutation_rate && mutated.size() >= 2) {
        const std::size_t n = mutated.size();

        // Escolhe um índice para remover
        std::uniform_int_distribution<std::size_t> distIdx(0, n - 1);
        std::size_t i = distIdx(rng);

        // Guarda o gene, remove e fecha o "buraco"
        int gene = mutated[i];
        mutated.erase(mutated.begin() + static_cast<std::ptrdiff_t>(i));

        // Escolhe posição para reinserir no vetor reduzido (0..size).
        // Inserção em 'pos' é antes do elemento atualmente em 'pos'.
        std::uniform_int_distribution<std::size_t> distPos(0, mutated.size());
        std::size_t pos = distPos(rng);

        // Evita no-op: se 'pos' corresponder exatamente à posição original,
        // ajusta uma casa (quando possível). Alternativamente, poderíamos reamostrar.
        if (pos == i) {
            if (pos < mutated.size()) ++pos;  // insere logo depois
            else if (pos > 0) --pos;          // ou logo antes, se pos==end()
        }

        mutated.insert(mutated.begin() + static_cast<std::ptrdiff_t>(pos), gene);
    }

    return mutated;
}

std::vector<int> simple_inversion_mutation(const std::vector<int>& individual, double mutation_rate, std::mt19937& rng) {
    if (mutation_rate < 0.0 || mutation_rate > 1.0) {
        throw std::invalid_argument("mutation_rate must be between 0 and 1.");
    }

    std::vector<int> mutated = individual;

    std::uniform_real_distribution<double> dist01(0.0, 1.0);

    if (dist01(rng) < mutation_rate && mutated.size() >= 2) {
        const std::size_t n = mutated.size();

        // Seleciona dois pontos de corte distintos
        std::uniform_int_distribution<std::size_t> distL(0, n - 2);
        std::size_t l = distL(rng);

        std::uniform_int_distribution<std::size_t> distR(l + 1, n - 1);
        std::size_t r = distR(rng);

        // Inverte o segmento entre l e r (inclusive)
        std::reverse(mutated.begin() + static_cast<std::ptrdiff_t>(l),
                     mutated.begin() + static_cast<std::ptrdiff_t>(r) + 1);
    }

    return mutated;
}