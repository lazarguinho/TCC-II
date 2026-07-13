"""
Gera 40 grafos Erdos-Renyi para comparação dos modelos PLI.
1 grafo por combinação (n, p): 10 tamanhos × 4 densidades = 40 grafos.
"""

import os
import random
import networkx as nx
from scipy.io import mmwrite
from scipy.sparse import csr_matrix

DENSITIES = [0.2, 0.4, 0.6, 0.8]
SIZES     = [100, 200, 300, 400, 500, 600, 700, 800, 900, 1000]
SEED      = 42

OUT_DIR = os.path.join(os.path.dirname(__file__), "..", "data", "pli_comparison")

os.makedirs(OUT_DIR, exist_ok=True)
random.seed(SEED)

for n in SIZES:
    for p in DENSITIES:
        G = nx.erdos_renyi_graph(n, p, seed=random.randint(0, 2**31))
        name = f"erdos_n{n}_p{p}.mtx"
        path = os.path.join(OUT_DIR, name)
        A = nx.to_scipy_sparse_array(G, dtype=int)
        mmwrite(path, csr_matrix(A))
        print(f"Gerado: {path}")

print(f"\nConcluído: {len(SIZES) * len(DENSITIES)} grafos em {OUT_DIR}")
