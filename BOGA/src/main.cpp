#include <iostream>
#include <vector>
#include <numeric>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <functional>
#include <cstdlib>
#include <limits>
#include <filesystem>

#include "ga.hpp"
#include "selection.hpp"
#include "crossover.hpp"
#include "mutation.hpp"
#include "greedy.hpp"

using Graph = std::vector<std::vector<int>>;

// ===== Leitora simples de .mtx (Coordinate format) =====
Graph read_mtx_graph(const std::string& filename, bool undirected = true) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Erro ao abrir arquivo .mtx: " + filename);
    }

    std::string line;
    // pular comentários (%)
    while (std::getline(file, line)) {
        if (!line.empty() && line[0] != '%') break;
    }

    // dimensões
    std::istringstream dims(line);
    int rows = 0, cols = 0, edges = 0;
    dims >> rows >> cols >> edges;

    if (rows <= 0 || cols <= 0) {
        throw std::runtime_error("Dimensões inválidas no cabeçalho do .mtx");
    }

    Graph g(rows);
    int u, v;
    for (int i = 0; i < edges; ++i) {
        file >> u >> v;
        // converter para 0-based
        --u; --v;
        if (u < 0 || u >= rows || v < 0 || v >= rows) continue;
        g[u].push_back(v);
        if (undirected && u != v) g[v].push_back(u);
    }
    return g;
}

// Tipos de operadores do GA
using CrossoverOp = std::function<
    std::pair<std::vector<int>, std::vector<int>>(
        const std::vector<int>&,
        const std::vector<int>&
    )
>;
using MutationOp = std::function<
    std::vector<int>(
        const std::vector<int>&,
        double,
        std::mt19937&
    )
>;

template <class T>
void print_vec(std::ofstream& out, const std::vector<T>& v, const char* name) {
    out << name << " = [";
    for (size_t i = 0; i < v.size(); ++i) {
        out << v[i];
        if (i + 1 < v.size()) out << ", ";
    }
    out << "]\n";
}

struct Options {
    std::string file;
    std::string opx = "cx";   // crossover
    std::string opm = "em";   // mutation
    bool undirected = true;  // default: undirected
    size_t pop = 20;
    size_t gens = 100;
    double mut = 0.10;
    double cr = 1.0;
    size_t runs = 1;
    bool show_help = false;
};

void print_usage(const char* prog) {
    std::cout <<
R"(Uso:
  )" << prog << R"( <caminho.mtx> [opções]

Posicional:
  <caminho.mtx>            Arquivo Matrix Market (.mtx)

Opções:
  --opx, -x <cx|ox|pmx>    Operador de crossover (padrão: cx)
  --opm, -m <em|dm|ivm|sm|ism|sim>
                           Operador de mutação (padrão: em)
  --pop <N>                Tamanho da população (padrão: 20)
  --gens <N>               Número de gerações (padrão: 100)
  --mut <r>                Taxa de mutação [0..1] (padrão: 0.10)
  --cr <r>                 Taxa de cruzamento [0..1] (padrão: 1.0)
  --runs <N>               Número de execuções (padrão: 1)
  --undirected, -u         Lê o grafo como não-direcionado
  --directed, -d           Lê o grafo como direcionado (padrão)
  --help, -h               Mostra esta ajuda

Exemplo:
  )" << prog << R"( data/NR_DIMACS/johnson8-2-4.mtx --opx cx --opm em --pop 50 --gens 300 --mut 0.15 --runs 5 --undirected
)";
}

Options parse_args(int argc, char** argv) {
    Options opt;
    auto need_value = [&](int& i, const char* flag){
        if (i + 1 >= argc) throw std::runtime_error(std::string("Faltando valor para ")+flag);
        return std::string(argv[++i]);
    };

    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "-h" || a == "--help") {
            opt.show_help = true;
        } else if (a == "-x" || a == "--opx") {
            opt.opx = need_value(i, a.c_str());
        } else if (a == "-m" || a == "--opm") {
            opt.opm = need_value(i, a.c_str());
        } else if (a == "--pop") {
            opt.pop = static_cast<size_t>(std::stoul(need_value(i, a.c_str())));
        } else if (a == "--gens") {
            opt.gens = static_cast<size_t>(std::stoul(need_value(i, a.c_str())));
        } else if (a == "--mut") {
            opt.mut = std::stod(need_value(i, a.c_str()));
            if (opt.mut < 0.0 || opt.mut > 1.0) throw std::runtime_error("--mut fora do intervalo [0,1]");
        } else if (a == "--cr") {
            opt.cr = std::stod(need_value(i, a.c_str()));
            if (opt.cr < 0.0 || opt.cr > 1.0) throw std::runtime_error("--cr fora do intervalo [0,1]");
        } else if (a == "--runs") {
            opt.runs = static_cast<size_t>(std::stoul(need_value(i, a.c_str())));
            if (opt.runs < 1) throw std::runtime_error("--runs deve ser >= 1");
        } else if (a == "-u" || a == "--undirected") {
            opt.undirected = true;
        } else if (a == "-d" || a == "--directed") {
            opt.undirected = false;
        } else if (!a.empty() && a[0] == '-') {
            throw std::runtime_error("Flag desconhecida: " + a);
        } else {
            // primeiro argumento não-flag é o arquivo
            if (opt.file.empty()) opt.file = a;
            else throw std::runtime_error("Vários caminhos de arquivo informados: " + opt.file + " e " + a);
        }
    }
    return opt;
}

int main(int argc, char** argv) {
    try {
        Options opt = parse_args(argc, argv);
        if (opt.show_help || opt.file.empty()) {
            print_usage(argv[0]);
            return opt.show_help ? 0 : 1;
        }

        std::filesystem::create_directories("results");
        std::string base = std::filesystem::path(opt.file).filename().string();
        std::string outname = "results/" + base + "_result.txt";

        std::ofstream out(outname);
        if (!out.is_open()) {
            throw std::runtime_error("Erro ao criar arquivo de saída: " + outname);
        }

        // ---- Mapear strings -> operadores ----
        CrossoverOp cross = cycle_crossover;  // default
        if      (opt.opx == "cx")  cross = cycle_crossover;
        else if (opt.opx == "ox")  cross = order_crossover;
        else if (opt.opx == "pmx") cross = partially_mapped_crossover;
        else std::cerr << "Aviso: crossover inválido '" << opt.opx << "', usando 'cx'.\n";

        MutationOp mut_op = exchange_mutation; // default
        if      (opt.opm == "em")  mut_op = exchange_mutation;
        else if (opt.opm == "dm")  mut_op = displacement_mutation;
        else if (opt.opm == "ivm") mut_op = inversion_mutation;
        else if (opt.opm == "sm")  mut_op = scramble_mutation;
        else if (opt.opm == "ism") mut_op = insertion_mutation;
        else if (opt.opm == "sim") mut_op = simple_inversion_mutation;
        else std::cerr << "Aviso: mutação inválida '" << opt.opm << "', usando 'em'.\n";

        // ---- Ler grafo ----
        Graph g = read_mtx_graph(opt.file, opt.undirected);

        // ---- Parâmetros do GA ----
        const int n = static_cast<int>(g.size());
        std::vector<int> node_ids(n);
        std::iota(node_ids.begin(), node_ids.end(), 0);

        out << "===== RESULTADO GA L(2,1) =====\n";
        out << "Arquivo: " << opt.file << (opt.undirected ? " (undirected)\n" : " (directed)\n");
        out << "Crossover: " << opt.opx << " | Mutação: " << opt.opm
                  << " | pop=" << opt.pop << " | gens=" << opt.gens << " | mut=" << opt.mut << " | cr=" << opt.cr << " | runs=" << opt.runs << "\n";

        GAResult best_overall;
        int best_span = std::numeric_limits<int>::max();
        bool has_best = false;

        for (size_t r = 0; r < opt.runs; ++r) {
            out << "\n----- Execução #" << r + 1 << " -----\n";
            
            GAResult res = genetic_algorithm_labeling(
                g,
                node_ids,
                opt.pop,
                opt.gens,
                opt.mut,
                opt.cr,
                cross,
                mut_op
            );

            out << "Span (lambda): " << res.span_value << "\n";
            print_vec(out, res.best_order, "best_order");
            print_vec(out, res.labeling,   "labeling");
            print_vec(out, res.span_per_generation, "convergência");

            if (res.span_value < best_span) {
                best_span = res.span_value;
                best_overall = res;
                has_best = true;
            }

            out << "================================\n";
        }

        // ---- Executa GA ----
        
        
        out << "\n----- Melhor execução -----\n";
        if (has_best) {
            out << "Span (lambda): " << best_span << "\n";
            std::cout << best_span << "\n";
            print_vec(out, best_overall.best_order, "best_order");
            print_vec(out, best_overall.labeling,   "labeling");
        } else {
            out << "Nenhum resultado encontrado.\n";
        }

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Erro: " << e.what() << '\n';
        return 1;
    }
}