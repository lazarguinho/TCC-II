#ifndef DECODER_H
#define DECODER_H

#include <vector>
#include <cstddef>
#include <atomic>
#include <chrono>
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

    // Controle de tempo: define prazo absoluto e verifica se foi ultrapassado.
    // Seguro para múltiplas threads (OpenMP): deadline_ é escrita uma vez antes
    // do paralelismo; time_expired_ é atômica.
    void set_deadline(std::chrono::high_resolution_clock::time_point dl) const {
        deadline_ = dl;
        time_expired_.store(false, std::memory_order_relaxed);
    }
    bool time_expired() const { return time_expired_.load(std::memory_order_relaxed); }

private:
    const Graph& g;

    // mutable: permite atualizar mesmo dentro de métodos const
    mutable std::vector<int> best_order_;
    mutable std::vector<int> labeling_;
    mutable std::vector<int> convergence_;

    // Prazo absoluto; inicializado com max() → sem limite por padrão
    mutable std::chrono::high_resolution_clock::time_point deadline_{
        std::chrono::high_resolution_clock::time_point::max()
    };
    // Flag thread-safe: setado por qualquer thread de decode() ao detectar timeout
    mutable std::atomic<bool> time_expired_{false};
};

#endif