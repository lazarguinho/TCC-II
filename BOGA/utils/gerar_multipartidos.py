"""
Gera grafos k-partidos completos balanceados K_{s,s,...,s} (k partes de s vértices).

Baseado na Tabela 2 do artigo:
  "Solving L(2,1)-labeling Problem of Graphs using Genetic Algorithms" (Han et al.)

Para K_{s,...,s} com k partes:
  - Vértices totais: n = k * s
  - Arestas: entre todo par de vértices de PARTES DIFERENTES
  - m = C(k,2) * s^2 = k*(k-1)/2 * s^2
  - Solução ótima L(2,1): lambda(G) = n + k - 2

Os três grafos do artigo (conforme multipartidos.md):
  partite1: k=5, s=6   -> K_{6,6,6,6,6}           | 30 vértices,  360 arestas, lambda=33
  partite2: k=5, s=10  -> K_{10,10,10,10,10}       | 50 vértices, 1000 arestas, lambda=53
  partite3: k=10, s=5  -> K_{5,5,5,5,5,5,5,5,5,5} | 50 vértices, 1125 arestas, lambda=58

Formato de saída: Matrix Market coordinate integer general
  - Compatível com BRKGA (lê 'u v val')
  - BOGA lê 'u v' apenas; use --undirected e o leitor do BOGA ignora val se adaptado.
"""

import os
from itertools import combinations

OUTPUT_DIR = os.path.dirname(os.path.abspath(__file__))

GRAPHS = [
    # (nome, k, s)  -- conforme Tabela 2 de Han et al. (ver multipartidos.md)
    ("partite1_K6x5",  5,  6),   # K_{6,6,6,6,6}           | 30 vértices,  360 arestas, lambda=33
    ("partite2_K10x5", 5, 10),   # K_{10,10,10,10,10}       | 50 vértices, 1000 arestas, lambda=53
    ("partite3_K5x10", 10, 5),   # K_{5,5,5,5,5,5,5,5,5,5} | 50 vértices, 1125 arestas, lambda=58
]


def generate_kpartite(k: int, s: int):
    """Retorna arestas duplicadas (u,v) e (v,u) 1-indexadas para K_{s,...,s} com k partes."""
    edges = []
    # parte i: vértices [(i-1)*s + 1 .. i*s]  (1-indexed)
    parts = [range((i - 1) * s + 1, i * s + 1) for i in range(1, k + 1)]
    for p1, p2 in combinations(range(k), 2):
        for u in parts[p1]:
            for v in parts[p2]:
                edges.append((u, v))
                edges.append((v, u))
    return edges


def write_mtx(filepath: str, k: int, s: int):
    n = k * s
    edges = generate_kpartite(k, s)
    m = len(edges)
    lambda_opt = n + k - 2

    print(f"Gerando {os.path.basename(filepath)}")
    print(f"  k={k} partes, s={s} vértices/parte")
    print(f"  n={n} vértices, m={m} arestas, lambda(G)={lambda_opt}")

    with open(filepath, "w") as f:
        f.write("%%MatrixMarket matrix coordinate integer general\n")
        f.write(f"% K_{{{s},...,{s}}} com k={k} partes | n={n} v | m={m} e | lambda={lambda_opt}\n")
        f.write(f"{n} {n} {m}\n")
        for u, v in edges:
            f.write(f"{u} {v} 1\n")

    print(f"  -> Salvo em: {filepath}\n")


def main():
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    for name, k, s in GRAPHS:
        filepath = os.path.join(OUTPUT_DIR, f"{name}.mtx")
        write_mtx(filepath, k, s)

    print("Pronto! Verifique os parametros contra a Tabela 2 do artigo.")
    print("Se necessário, edite a lista GRAPHS neste script para corrigir k e s.")


if __name__ == "__main__":
    main()
