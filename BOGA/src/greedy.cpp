#include "greedy.hpp"
#include <algorithm>
#include <limits>
#include <iostream>

#define DEBUG 0
#if DEBUG
  #define DOUT std::cout
#else
  #define DOUT if (false) std::cout
#endif

GreedyResult greedy_labeling(const Graph& graph, const std::vector<int>& order) {
    const int n = static_cast<int>(order.size());
    DOUT << "Número de vértices (n): " << n << '\n';

    const int max_label_bound = std::max(1, 2 * n - 2); 
    DOUT << "Limite máximo de rótulos (max_label_bound): " << max_label_bound << '\n';

    std::vector<int> f(n, -1);
    DOUT << "Vetor f inicial: [";
    for (int i = 0; i < n; ++i) {
        DOUT << f[i];
        if (i < n - 1) DOUT << ", ";
    }
    DOUT << "]\n";

    int k = 0;
    DOUT << "Valor inicial de k: " << k << '\n';

    DOUT << "Para cada vértice:\n";
    for (int v_idx = 0; v_idx < n; ++v_idx) {
        int v = order[v_idx];
        DOUT << "\t-----Vértice " << v << "-----\n";

        std::vector<char> banned(max_label_bound, false);
        DOUT << "\tbanned: [";
        for (int i = 0; i < max_label_bound; ++i) {
            DOUT << (banned[i] ? "sim" : "não");
            if (i < max_label_bound - 1) DOUT << ", ";
        }
        DOUT << "]\n";

        DOUT << "\tPara cada visinho de " << v << " [";
        for (unsigned long i = 0; i < graph[v].size(); ++i) {
            DOUT << graph[v][i];
            if (i < graph[v].size() - 1) DOUT << ", ";
        }
        DOUT << "]:\n";
        for (int neighbor : graph[v]) {
            DOUT << "\t\tO vértice " << neighbor << " tem rótulo atribuido? ";
            if (neighbor >= 0 && neighbor < n && f[neighbor] != -1) {
                DOUT << "Sim!\n";
                int lu = f[neighbor];
                DOUT << "\t\t\tComo o rótulo de " << neighbor << " é " << lu << '\n';
                if (lu >= 0 && lu < max_label_bound) banned[lu] = true;
                if (lu - 1 >= 0 && lu - 1 < max_label_bound) banned[lu - 1] = true;
                if (lu + 1 >= 0 && lu + 1 < max_label_bound) banned[lu + 1] = true;
                DOUT << "\t\t\t\tOs rótulos " << lu - 1 << ", " << lu << " e " << lu + 1 << " estao banidos\n";
            } else {
                DOUT << "Não!\n";
            }

            DOUT << "\t\tPara cada vizinho de " << neighbor << " que é um vizinho de " << v << " [";
            for (unsigned long i = 0; i < graph[neighbor].size(); ++i) {
                DOUT << graph[neighbor][i];
                if (i < graph[neighbor].size() - 1) DOUT << ", ";
            }
            DOUT << "]:\n";
            for (int w : graph[neighbor]) {
                DOUT << "\t\t\tO vértice " << w << " tem rótulo atribuido? ";
                if (w >= 0 && w < n && f[w] != -1) {
                    DOUT << "Sim!\n";
                    int lw = f[w];
                    DOUT << "\t\t\t\tComo o rótulo de " << w << " é " << lw << '\n';
                    if (lw >= 0 && lw < max_label_bound) banned[lw] = true;
                    DOUT << "\t\t\t\t\tO rótulo " << lw << " está banido\n";
                } else {
                    DOUT << "Não!\n";
                }
            }
        }

        int chosen = 0;
        DOUT << "\t\tRótulo escolhido começando em " << chosen << "\n";

        DOUT << "\t\tEnquanto rótulo escolhido estiver banido vá para o próximo rótulo, mas pare caso chegue ao rótulo máximo " << max_label_bound << '\n';
        while (chosen < max_label_bound && banned[chosen]) ++chosen;

        DOUT << "\t\tRótulo escolhido: " << chosen << '\n';

        f[v] = chosen;
        DOUT << "\t\tAtribui rótulo " << chosen << " ao vértice " << v << "\n";

        k = std::max(k, chosen);
        DOUT << "\t\tAtribui rótulo " << k << " ao k\n";

        DOUT << "\t------- Fim da iteração " << v_idx << " -------\n\n\n";
    }

    return GreedyResult{ k, std::move(f) };
}
