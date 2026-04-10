#include "Graph.h"

#include <stdexcept>
#include <algorithm>

Graph::Graph(std::size_t n) : adj_(n) {}

std::size_t Graph::size() const noexcept {
    return adj_.size();
}

bool Graph::empty() const noexcept {
    return adj_.empty();
}

void Graph::reset(std::size_t n) {
    adj_.assign(n, {});
}

void Graph::checkVertex(int u) const {
    if (u < 0 || static_cast<std::size_t>(u) >= adj_.size()) {
        throw std::out_of_range("Graph: vertice fora do intervalo");
    }
}

void Graph::addEdge(int u, int v) {
    checkVertex(u);
    checkVertex(v);
    if (u == v) return; // ignora auto-laço (se quiser permitir, remova)

    // evita duplicatas (caso seu input possa repetir arestas)
    auto& Nu = adj_[static_cast<std::size_t>(u)];
    if (std::find(Nu.begin(), Nu.end(), v) == Nu.end()) {
        Nu.push_back(v);
    }

    auto& Nv = adj_[static_cast<std::size_t>(v)];
    if (std::find(Nv.begin(), Nv.end(), u) == Nv.end()) {
        Nv.push_back(u);
    }
}

const std::vector<int>& Graph::neighbors(int u) const {
    checkVertex(u);
    return adj_[static_cast<std::size_t>(u)];
}

std::size_t Graph::degree(int u) const {
    return neighbors(u).size();
}