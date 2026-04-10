#include "decoder.h"

#include <numeric>   // iota
#include <algorithm> // stable_sort
#include <stdexcept>

static bool validate_L21(
    const Graph& g,
    const std::vector<int>& lab,
    int& out_u, int& out_v, int& out_dist, int& out_diff
) {
    const int n = (int)g.size();
    if ((int)lab.size() != n) return false;

    // Distância 1: |f(u)-f(v)| >= 2
    for (int u = 0; u < n; ++u) {
        for (int v : g[u]) {
            if (v < 0 || v >= n) continue;
            if (u == v) continue; // laço: ignore ou trate como erro
            int diff = std::abs(lab[u] - lab[v]);
            if (diff < 2) {
                out_u = u; out_v = v; out_dist = 1; out_diff = diff;
                return false;
            }
        }
    }

    // Distância 2: |f(u)-f(w)| >= 1 para todo w a distância 2
    // Faz via: para cada u, percorre vizinhos v e depois vizinhos w de v
    for (int u = 0; u < n; ++u) {
        for (int v : g[u]) {
            if (v < 0 || v >= n) continue;
            for (int w : g[v]) {
                if (w < 0 || w >= n) continue;
                if (w == u) continue;
                // se w é vizinho direto também, a restrição de dist1 já cobre (>=2),
                // mas a de dist2 (>=1) não atrapalha. Pode checar mesmo assim.
                int diff = std::abs(lab[u] - lab[w]);
                if (diff < 1) { // ou seja, diff == 0
                    out_u = u; out_v = w; out_dist = 2; out_diff = diff;
                    return false;
                }
            }
        }
    }

    return true;
}

double Decoder::decode(const std::vector<double>& chrom) const {
    const std::size_t n = g.size();

    if (chrom.size() != n) {
        throw std::runtime_error(
            "Decoder::decode: tamanho do cromossomo (" + std::to_string(chrom.size()) +
            ") != numero de vertices do grafo (" + std::to_string(n) + ")."
        );
    }

    // 1) best_order
    best_order_.resize(n);
    std::iota(best_order_.begin(), best_order_.end(), 0);

    std::stable_sort(best_order_.begin(), best_order_.end(),
        [&](int i, int j) { return chrom[(std::size_t)i] < chrom[(std::size_t)j]; });

    // 2) greedy
    GreedyResult gr = greedy_labeling(g, best_order_);

    // 3) SALVA antes de validar (ou valida gr.labels direto)
    labeling_ = gr.labels;

    // 4) valida a rotulação calculada
    int cu=-1, cv=-1, cdist=-1, cdiff=-1;
    if (!validate_L21(g, labeling_, cu, cv, cdist, cdiff)) {
        // dica: loga pra depurar
        // std::cerr << "Invalida: u="<<cu<<" v="<<cv<<" dist="<<cdist<<" diff="<<cdiff<<"\n";
        return 1e18;
    }

    return static_cast<double>(gr.k);
}