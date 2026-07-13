"""
Gera grafos Cubo de Fibonacci Gamma_n (n=2..16) em formato Matrix Market.

Gamma_n:
  - Vértices: strings binárias de comprimento n sem dois 1's consecutivos
  - Arestas: dois vértices são adjacentes se diferirem em exatamente 1 bit
  - |V| = F(n+2)  (sequência de Fibonacci)

Formato de saída: %%MatrixMarket matrix coordinate integer general
  Cada aresta listada duas vezes (u v e v u), consistente com os
  demais scripts da base (graph_cleaning.py, generate_pli_graphs.py).
"""

import os
from itertools import combinations

OUTPUT_DIR = os.path.dirname(os.path.abspath(__file__))


def fibonacci_cube_vertices(n: int) -> list[str]:
    """Retorna lista de strings binárias de comprimento n sem dois 1's consecutivos."""
    return [
        format(i, f"0{n}b")
        for i in range(2**n)
        if "11" not in format(i, f"0{n}b")
    ]


def hamming_distance_1(a: str, b: str) -> bool:
    return sum(x != y for x, y in zip(a, b)) == 1


def generate_edges(vertices: list[str]) -> list[tuple[int, int]]:
    """Retorna arestas (1-indexed) duplicadas (u,v) e (v,u) onde vértices diferem em 1 bit."""
    edges = []
    for i, j in combinations(range(len(vertices)), 2):
        if hamming_distance_1(vertices[i], vertices[j]):
            edges.append((i + 1, j + 1))
            edges.append((j + 1, i + 1))
    return edges


def write_mtx(n: int):
    vertices = fibonacci_cube_vertices(n)
    edges = generate_edges(vertices)
    num_v = len(vertices)
    num_e = len(edges)

    filename = f"gamma_{n}.mtx"
    filepath = os.path.join(OUTPUT_DIR, filename)

    with open(filepath, "w") as f:
        f.write("%%MatrixMarket matrix coordinate integer general\n")
        f.write(f"% Cubo de Fibonacci Gamma_{n} | {num_v} vertices | {num_e} arestas\n")
        f.write(f"{num_v} {num_v} {num_e}\n")
        for u, v in edges:
            f.write(f"{u} {v} 1\n")

    print(f"gamma_{n:2d}.mtx  |  {num_v:5d} vértices  |  {num_e:6d} arestas")


def main():
    print(f"{'Arquivo':<15}  {'Vértices':>10}  {'Arestas':>10}")
    print("-" * 40)
    for n in range(2, 17):
        write_mtx(n)
    print(f"\nArquivos salvos em: {OUTPUT_DIR}")


if __name__ == "__main__":
    main()
