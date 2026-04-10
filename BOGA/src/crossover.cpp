#include "crossover.hpp"

#include <unordered_map>
#include <unordered_set>
#include <stdexcept>
#include <cstddef>
#include <algorithm>
#include <random>
#include <limits>
#include <iostream>
#include <iomanip>

// ------------------------------------------------------------
// util: sorteia limites [l, r] distintos
// ------------------------------------------------------------
static inline std::pair<std::size_t, std::size_t>
pick_segment_bounds(std::size_t n, std::mt19937 &rng)
{
    if (n < 2)
        return {0, 0};
    std::uniform_int_distribution<std::size_t> distL(0, n - 2);
    std::size_t l = distL(rng);
    std::uniform_int_distribution<std::size_t> distR(l + 1, n - 1);
    std::size_t r = distR(rng);
    return {l, r};
}

// ------------------------------------------------------------
// Cycle Crossover (CX)
// ------------------------------------------------------------
std::pair<std::vector<int>, std::vector<int>>
cycle_crossover(const std::vector<int> &p1, const std::vector<int> &p2)
{
    const std::size_t n = p1.size();
    if (n == 0 || p2.size() != n)
    {
        throw std::invalid_argument("cycle_crossover: parents must be non-empty and same length.");
    }

    // mapeia valor->posição em p2
    std::unordered_map<int, std::size_t> pos2;
    pos2.reserve(n);
    for (std::size_t i = 0; i < n; ++i)
        pos2[p2[i]] = i;

    // checa compatibilidade de conjuntos
    for (int v : p1)
    {
        if (!pos2.count(v))
        {
            throw std::invalid_argument("cycle_crossover: parents must contain the same set of values.");
        }
    }

    std::vector<int> c1(n), c2(n);
    std::vector<bool> visited(n, false);

    bool take_from_p1 = true;
    for (std::size_t start = 0; start < n; ++start)
    {
        if (visited[start])
            continue;

        std::size_t idx = start;
        do
        {
            visited[idx] = true;
            if (take_from_p1)
            {
                c1[idx] = p1[idx];
                c2[idx] = p2[idx];
            }
            else
            {
                c1[idx] = p2[idx];
                c2[idx] = p1[idx];
            }
            idx = pos2[p1[idx]];
        } while (idx != start);

        take_from_p1 = !take_from_p1; // alterna ciclo
    }

    return {c1, c2};
}

// ------------------------------------------------------------
// Order Crossover (OX)
// ------------------------------------------------------------
static inline void ox_one_child(const std::vector<int> &donor,
                                const std::vector<int> &other,
                                std::size_t l, std::size_t r,
                                std::vector<int> &child)
{
    const std::size_t n = donor.size();
    child.assign(n, std::numeric_limits<int>::min());

    // copia o segmento [l, r] do pai doador
    for (std::size_t i = l; i <= r; ++i)
        child[i] = donor[i];

    std::unordered_set<int> segment(donor.begin() + static_cast<std::ptrdiff_t>(l),
                                    donor.begin() + static_cast<std::ptrdiff_t>(r) + 1);

    // percorre o segundo pai circularmente e preenche os espaços vazios
    std::size_t pos = (r + 1) % n;
    for (std::size_t k = 0; k < n; ++k)
    {
        int gene = other[(r + 1 + k) % n];
        if (segment.count(gene))
            continue;
        while (child[pos] != std::numeric_limits<int>::min())
            pos = (pos + 1) % n;
        child[pos] = gene;
        pos = (pos + 1) % n;
    }
}

std::pair<std::vector<int>, std::vector<int>>
order_crossover(const std::vector<int> &p1, const std::vector<int> &p2)
{
    const std::size_t n = p1.size();
    if (n == 0 || p2.size() != n)
        throw std::invalid_argument("order_crossover: parents must be non-empty and same length.");

    // garante que sejam permutações compatíveis
    std::unordered_set<int> s1(p1.begin(), p1.end());
    for (int v : p2)
    {
        if (!s1.count(v))
        {
            throw std::invalid_argument("order_crossover: parents must contain the same set of values.");
        }
    }

    static thread_local std::mt19937 rng(std::random_device{}());
    auto [l, r] = pick_segment_bounds(n, rng);

    std::vector<int> c1, c2;
    ox_one_child(p1, p2, l, r, c1);
    ox_one_child(p2, p1, l, r, c2);

    return {c1, c2};
}

// ------------------------------------------------------------
// Partially Mapped Crossover (PMX)
// ------------------------------------------------------------
static inline void pmx_one_child(const std::vector<int> &donor,
                                 const std::vector<int> &other,
                                 std::size_t l, std::size_t r,
                                 std::vector<int> &child)
{
    const std::size_t n = donor.size();
    child.assign(n, std::numeric_limits<int>::min());

    // conjunto e mapas
    std::unordered_set<int> donorSeg(donor.begin() + static_cast<std::ptrdiff_t>(l),
                                     donor.begin() + static_cast<std::ptrdiff_t>(r) + 1);
    std::unordered_map<int, int> mapOtherToDonor;
    mapOtherToDonor.reserve(r - l + 1);

    // copia o segmento e cria o mapeamento bidirecional
    for (std::size_t i = l; i <= r; ++i)
        mapOtherToDonor[other[i]] = donor[i], child[i] = donor[i];

    auto resolve_conflict = [&](int g) -> int
    {
        while (mapOtherToDonor.count(g))
            g = mapOtherToDonor[g];
        return g;
    };

    // preenche o resto
    for (std::size_t i = 0; i < n; ++i)
    {
        if (i >= l && i <= r)
            continue;
        int cand = other[i];
        if (donorSeg.count(cand))
            cand = resolve_conflict(cand);
        child[i] = cand;
    }
}

std::pair<std::vector<int>, std::vector<int>>
partially_mapped_crossover(const std::vector<int> &p1, const std::vector<int> &p2)
{
    const std::size_t n = p1.size();
    if (n == 0 || p2.size() != n)
        throw std::invalid_argument("pmx: parents must be non-empty and same length.");

    // valida conjunto
    {
        std::unordered_set<int> s1(p1.begin(), p1.end());
        for (int v : p2)
            if (!s1.count(v))
            {
                std::cout << "p1 = " << std::setw(2) << p1[0];
                for (std::size_t i = 1; i < n; ++i)
                    std::cout << " " << std::setw(2) << p1[i];
                std::cout << std::endl
                          << "p2 = " << std::setw(2) << p2[0];
                for (std::size_t i = 1; i < n; ++i)
                    std::cout << " " << std::setw(2) << p2[i];
                std::cout << std::endl;
                throw std::invalid_argument("pmx: parents must contain the same set of values.");
            }
    }

    static thread_local std::mt19937 rng(std::random_device{}());
    auto [l, r] = pick_segment_bounds(n, rng);

    std::vector<int> c1, c2;
    pmx_one_child(p1, p2, l, r, c1);
    pmx_one_child(p2, p1, l, r, c2);

    return {c1, c2};
}