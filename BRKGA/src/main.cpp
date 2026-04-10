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
#include <chrono>

#include "greedy.hpp"
#include "decoder.h"
#include "brkgaAPI/BRKGA.h"
#include "brkgaAPI/MTRand.h"

using Graph = std::vector<std::vector<int>>;

// ===== Leitora simples de .mtx (Coordinate format) =====
Graph read_mtx_graph(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("Erro ao abrir arquivo .mtx: " + filename);

    std::string line;

    // 1) ler a primeira linha %%MatrixMarket
    if (!std::getline(file, line))
        throw std::runtime_error("Arquivo .mtx vazio: " + filename);

    // 2) pular comentários (%) até achar dimensões
    while (std::getline(file, line))
    {
        if (!line.empty() && line[0] != '%')
            break;
    }

    // dimensões
    std::istringstream dims(line);
    int rows = 0, cols = 0, nnz = 0;
    dims >> rows >> cols >> nnz;
    if (rows <= 0 || cols <= 0)
        throw std::runtime_error("Dimensões inválidas no cabeçalho do .mtx");

    Graph g(rows);

    int u, v;
    int val; // ignora o valor, mas precisa consumir!
    for (int i = 0; i < nnz; ++i)
    {
        if (!(file >> u >> v >> val))
            throw std::runtime_error("Falha ao ler entrada (esperava 3 colunas) em: " + filename);

        --u; --v;
        if (u < 0 || u >= rows || v < 0 || v >= rows) continue;

        // como é symmetric, espelha (e evita duplicar laço)
        g[u].push_back(v);
        if (u != v) g[v].push_back(u);
    }

    return g;
}

template <class T>
void print_vec(std::ofstream &out, const std::vector<T> &v, const char *name)
{
    out << name << " = [";
    for (size_t i = 0; i < v.size(); ++i)
    {
        out << v[i];
        if (i + 1 < v.size())
            out << ", ";
    }
    out << "]\n";
}

struct Options
{
    std::string file;
    unsigned n;          // size of chromosome
    unsigned p = 100;         // population size
    double pe = 0.2;          // elite set size
    double pm = 0.1;          // mutant set size
    double rhoe = 0.5;        // recombination probability
    unsigned K = 6;           // number of parallel populations
    unsigned MAXT = 6;        // maximum number of threads
    unsigned X_INTVL = 100;   // exchange best individual every X_INTVL generations
    unsigned X_NUMBER = 2;    // number of best individuals to exchange
    unsigned MAX_GENS = 1000; // maximum number of generations
    double MAX_STAGT = 400;   // maximum number of stagnation generations
    size_t runs = 1;
    bool show_help = false;
};

void print_usage(const char *prog)
{
    std::cout <<
        R"(Uso:
  )" << prog << R"( <caminho.mtx> [opções]

Posicional:
  <caminho.mtx>            Arquivo Matrix Market (.mtx)

Opções:
  -h, --help               Mostra esta ajuda

  --runs <N>               Número de execuções (padrão: 1)

  --p <N>                  population size
  --pe <x>                 elite fraction
  --pm <x>                 mutant fraction
  --rhoe <x>               recombination probability
  --K <N>                  number of parallel populations
  --MAXT <N>               maximum threads
  --X_INTVL <N>            exchange interval
  --X_NUMBER <N>           number of best individuals to exchange
  --MAX_GENS <N>           maximum generations
  --MAX_STAGT <x>          maximum stagnation gens

Exemplo:
  )" << prog << R"( data/NR_DIMACS/johnson8-2-4.mtx --runs 10 --out results.log
)";
}

static size_t to_size(const std::string &s, const std::string &flag_name)
{
    try
    {
        unsigned long long v = std::stoull(s);
        return static_cast<size_t>(v);
    }
    catch (...)
    {
        throw std::runtime_error("Valor inválido para " + flag_name + ": " + s);
    }
}

static unsigned to_u32(const std::string &s, const std::string &flag_name)
{
    try
    {
        unsigned long long v = std::stoull(s);
        if (v > std::numeric_limits<unsigned>::max())
            throw std::out_of_range("overflow");
        return static_cast<unsigned>(v);
    }
    catch (...)
    {
        throw std::runtime_error("Valor inválido para " + flag_name + ": " + s);
    }
}

static double to_double(const std::string &s, const std::string &flag_name)
{
    try
    {
        return std::stod(s);
    }
    catch (...)
    {
        throw std::runtime_error("Valor inválido para " + flag_name + ": " + s);
    }
}

Options parse_args(int argc, char **argv)
{
    Options opt;
    auto need_value = [&](int &i, const char *flag)
    {
        if (i + 1 >= argc)
            throw std::runtime_error(std::string("Faltando valor para ") + flag);
        return std::string(argv[++i]);
    };

    for (int i = 1; i < argc; ++i)
    {
        std::string a = argv[i];
        if (a == "-h" || a == "--help")
        {
            opt.show_help = true;
        }
        else if (a == "--runs")
        {
            std::string v = need_value(i, a.c_str());
            opt.runs = to_size(v, a);
        }
        else if (a == "--p")
        {
            opt.p = to_u32(need_value(i, a.c_str()), a);
        }
        else if (a == "--pe")
        {
            opt.pe = to_double(need_value(i, a.c_str()), a);
        }
        else if (a == "--pm")
        {
            opt.pm = to_double(need_value(i, a.c_str()), a);
        }
        else if (a == "--rhoe")
        {
            opt.rhoe = to_double(need_value(i, a.c_str()), a);
        }
        else if (a == "--K")
        {
            opt.K = to_u32(need_value(i, a.c_str()), a);
        }
        else if (a == "--MAXT")
        {
            opt.MAXT = to_u32(need_value(i, a.c_str()), a);
        }
        else if (a == "--X_INTVL")
        {
            opt.X_INTVL = to_u32(need_value(i, a.c_str()), a);
        }
        else if (a == "--X_NUMBER")
        {
            opt.X_NUMBER = to_u32(need_value(i, a.c_str()), a);
        }
        else if (a == "--MAX_GENS")
        {
            opt.MAX_GENS = to_u32(need_value(i, a.c_str()), a);
        }
        else if (a == "--MAX_STAGT")
        {
            opt.MAX_STAGT = to_double(need_value(i, a.c_str()), a);
        }
        else if (!a.empty() && a[0] == '-')
        {
            throw std::runtime_error("Flag desconhecida: " + a);
        }
        else
        {
            // primeiro argumento não-flag é o arquivo
            if (opt.file.empty())
                opt.file = a;
            else
                throw std::runtime_error("Vários caminhos de arquivo informados: " + opt.file + " e " + a);
        }
    }
    return opt;
}

int main(int argc, char **argv)
{
    try
    {
        Options opt = parse_args(argc, argv);
        if (opt.show_help || opt.file.empty())
        {
            print_usage(argv[0]);
            return opt.show_help ? 0 : 1;
        }
        double overall_best = std::numeric_limits<double>::max();

        std::filesystem::create_directories("results");
        std::string base = std::filesystem::path(opt.file).filename().string();
        std::string outname = "results/" + base + "_result.txt";

        std::ofstream out(outname);
        if (!out.is_open())
        {
            throw std::runtime_error("Erro ao criar arquivo de saída: " + outname);
        }

        // ---- Mapear strings -> operadores ----

        // ---- Ler grafo ----
        Graph g = read_mtx_graph(opt.file);

        out << "===== RESULTADO BRKGA L(2,1) =====\n";
        out << "Arquivo: " << opt.file << "\n";
        // out << "Crossover: " << opt.opx << " | Mutação: " << opt.opm
        //     << " | pop=" << opt.pop << " | gens=" << opt.gens << " | mut=" << opt.mut << " | cr=" << opt.cr << " | runs=" << opt.runs << "\n";

        for (size_t r = 0; r < opt.runs; ++r)
        {
            out << "\n----- Execução #" << r + 1 << " -----\n";

            Decoder decoder(g);
            decoder.clear_convergence();

            const long unsigned rngSeed = r;
            MTRand rng(rngSeed);

            opt.n = static_cast<unsigned>(g.size());

            BRKGA<Decoder, MTRand> ga(opt.n, opt.p, opt.pe, opt.pm, opt.rhoe, decoder, rng, opt.K, opt.MAXT);

            auto begin = std::chrono::high_resolution_clock::now();
            const auto TIME_LIMIT = std::chrono::minutes(15);

            unsigned generation = 0;
            unsigned stagnant_count = 0;

            double bestFitness = std::numeric_limits<double>::max();

            do
            {
                ga.evolve();

                if (bestFitness > ga.getBestFitness())
                {
                    bestFitness = ga.getBestFitness();
                    stagnant_count = 0;
                }
                else
                {
                    stagnant_count++;
                }

                decoder.push_convergence(static_cast<int>(bestFitness));

                if ((++generation) % opt.X_INTVL == 0)
                {
                    ga.exchangeElite(opt.X_NUMBER);
                }

                auto now = std::chrono::high_resolution_clock::now();
                if (now - begin >= TIME_LIMIT) {
                    out << "[INFO] Time limit atingido: 15 minutos. Parando a run.\n";
                    break;
                }

            } while (generation < opt.MAX_GENS && stagnant_count < opt.MAX_STAGT);

            if (generation >= opt.MAX_GENS) out << "[INFO] Parou por MAX_GENS.\n";
            else if (stagnant_count >= opt.MAX_STAGT) out << "[INFO] Parou por estagnação.\n";
            // se caiu no time limit, já escreveu a mensagem no break
            
            auto end = std::chrono::high_resolution_clock::now();
            auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);

            out << "Tempo de execução: " << elapsed_time.count() << "ms\n";
            const std::vector<double> bestChromosome = ga.getBestChromosome();
            const double best = ga.getBestFitness();

            decoder.decode(bestChromosome);

            overall_best = std::min(overall_best, best);

            out << "Span (lambda): " << best << "\n";
            print_vec(out, decoder.best_order(), "best_order");
            print_vec(out, decoder.labeling(), "labeling");
            print_vec(out, decoder.convergence(), "convergência");
        }

        out << "\n===== FIM =====\n";
        std::cout << overall_best;

        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Erro: " << e.what() << '\n';
        return 1;
    }
}