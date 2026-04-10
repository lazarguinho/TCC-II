#ifndef L21_AGBO_GRAPH_H
#define L21_AGBO_GRAPH_H

#include <vector>
#include <cstddef>

class Graph {
public:
    Graph() = default;
    explicit Graph(std::size_t n);

    std::size_t size() const noexcept;
    bool empty() const noexcept;

    // Redimensiona e apaga arestas existentes
    void reset(std::size_t n);

    // Grafo não-direcionado (o mais comum em L(2,1))
    void addEdge(int u, int v);

    // Acesso aos vizinhos
    const std::vector<int>& neighbors(int u) const;

    // (Opcional) graus
    std::size_t degree(int u) const;

private:
    std::vector<std::vector<int>> adj_;

    void checkVertex(int u) const;
};

#endif