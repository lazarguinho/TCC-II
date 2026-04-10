#ifndef DECODER_H
#define DECODER_H

#include <vector>
#include <cstddef>
#include "greedy.hpp"

using Graph = std::vector<std::vector<int>>;

class Decoder {
public:
    explicit Decoder(const Graph& graph) : g(graph) {}
    ~Decoder() = default;

    // PRECISA ser const por causa do BRKGA.h
    double decode(const std::vector<double>& chrom) const;

    // Getters pro main imprimir
    const std::vector<int>& best_order() const { return best_order_; }
    const std::vector<int>& labeling()   const { return labeling_; }
    const std::vector<int>& convergence() const { return convergence_; }

    // Se você quiser alimentar convergência por geração no main:
    void clear_convergence() const { convergence_.clear(); }
    void push_convergence(int best_lambda_so_far) const { convergence_.push_back(best_lambda_so_far); }

private:
    const Graph& g;

    // mutable: permite atualizar mesmo dentro de métodos const
    mutable std::vector<int> best_order_;
    mutable std::vector<int> labeling_;
    mutable std::vector<int> convergence_;
};

#endif