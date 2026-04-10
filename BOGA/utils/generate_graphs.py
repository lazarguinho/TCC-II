import os
import random
import shutil

import networkx as nx
from scipy.io import mmwrite
from scipy.sparse import csr_matrix

# ============================================================
# CONFIGURAÇÕES
# ============================================================

TOTAL_GRAPHS = 100
CALIBRATION_SIZE = 25 
# mudar para 10
# mudar para 200 no total

DENSITIES = [0.2, 0.4, 0.6, 0.8]
N_MIN, N_MAX = 100, 1000

# Faixas de n (ajuste como quiser)
N_BINS = [
    (100, 250),
    (251, 500),
    (501, 750),
    (751, 1000),
]

OUT_RANDOM = "data/random"
OUT_CALIB = "data/calibration"

SEED = 42 

# ============================================================
# Funções utilitárias
# ============================================================

def save_mtx(G, filename):
    A = nx.to_scipy_sparse_array(G, dtype=int)
    mmwrite(filename, csr_matrix(A))

def generate_erdos(n, p, filename):
    G = nx.erdos_renyi_graph(n, p)
    save_mtx(G, filename)

def bin_for_n(n):
    for (lo, hi) in N_BINS:
        if lo <= n <= hi:
            return (lo, hi)
    return None  # não deve acontecer se N_BINS cobrir [N_MIN, N_MAX]

def safe_sample(population, k):
    """Amostra sem reposição até onde dá; se k > len(pop), reduz."""
    if k <= 0:
        return []
    k = min(k, len(population))
    return random.sample(population, k)

# ============================================================
# MAIN
# ============================================================

if __name__ == "__main__":
    if SEED is not None:
        random.seed(SEED)

    os.makedirs(OUT_RANDOM, exist_ok=True)
    os.makedirs(OUT_CALIB, exist_ok=True)

    generated = []  # lista de (n, p, path)

    # ------------------------------------------------------------
    # Geração balanceada por densidade
    # ------------------------------------------------------------
    per_density = TOTAL_GRAPHS // len(DENSITIES)
    remainder = TOTAL_GRAPHS % len(DENSITIES)

    # Se houver remainder, distribui a sobra de forma aleatória
    densities_for_generation = DENSITIES[:]
    random.shuffle(densities_for_generation)

    for idx, p in enumerate(densities_for_generation):
        count = per_density + (1 if idx < remainder else 0)

        for _ in range(count):
            n = random.randint(N_MIN, N_MAX)
            name = f"erdos_n{n}_p{p}.mtx"
            path = os.path.join(OUT_RANDOM, name)

            generate_erdos(n, p, path)
            generated.append((n, p, path))
            print(f"Gerado: {path}")

    # ------------------------------------------------------------
    # SELEÇÃO ESTRATIFICADA: densidade + faixa de n
    # ------------------------------------------------------------
    print("\nSelecionando grafos para calibração (estratificado por p e bins de n)...\n")

    # Base por densidade e sobra distribuída aleatoriamente
    base_per_density = CALIBRATION_SIZE // len(DENSITIES)
    density_leftover = CALIBRATION_SIZE % len(DENSITIES)

    densities_for_calib = DENSITIES[:]
    random.shuffle(densities_for_calib)  # <- sobra vai pra densidades aleatórias

    density_quota = {}
    for i, p in enumerate(densities_for_calib):
        density_quota[p] = base_per_density + (1 if i < density_leftover else 0)

    # Dentro de cada densidade, distribuir quota pelos bins de n
    # Ex.: quota=7, bins=4 => [2,2,2,1] (com sobra aleatória)
    calibration_set = []
    selected_paths = set()  # evita duplicação

    for p in DENSITIES:
        q = density_quota[p]

        base_per_bin = q // len(N_BINS)
        bin_leftover = q % len(N_BINS)

        bins_order = N_BINS[:]
        random.shuffle(bins_order)  # <- sobra aleatória entre bins

        bin_quota = {}
        for j, b in enumerate(bins_order):
            bin_quota[b] = base_per_bin + (1 if j < bin_leftover else 0)

        # Selecionar por bin
        for b in N_BINS:
            k = bin_quota[b]
            lo, hi = b

            group = [
                item for item in generated
                if item[1] == p and lo <= item[0] <= hi and item[2] not in selected_paths
            ]

            picks = safe_sample(group, k)
            for (n, pp, path) in picks:
                calibration_set.append((n, pp, path))
                selected_paths.add(path)

        # Se faltou (porque algum bin não tinha suficientes), completa dentro da densidade
        if len([x for x in calibration_set if x[1] == p]) < q:
            missing = q - len([x for x in calibration_set if x[1] == p])
            pool = [item for item in generated if item[1] == p and item[2] not in selected_paths]
            picks = safe_sample(pool, missing)
            for (n, pp, path) in picks:
                calibration_set.append((n, pp, path))
                selected_paths.add(path)

    # Se por algum motivo ainda não bateu 25 (caso extremo), completa global
    if len(calibration_set) < CALIBRATION_SIZE:
        missing = CALIBRATION_SIZE - len(calibration_set)
        pool = [item for item in generated if item[2] not in selected_paths]
        picks = safe_sample(pool, missing)
        for (n, p, path) in picks:
            calibration_set.append((n, p, path))
            selected_paths.add(path)

    # Se passou (não deveria), corta
    calibration_set = calibration_set[:CALIBRATION_SIZE]

    # ------------------------------------------------------------
    # Copiar arquivos para calibração
    # ------------------------------------------------------------
    for (_, _, path) in calibration_set:
        shutil.copy(path, OUT_CALIB)
        print("Copiado para calibração:", path)

    # ------------------------------------------------------------
    # Resumo
    # ------------------------------------------------------------
    from collections import Counter
    dens_count = Counter([p for (_, p, _) in calibration_set])
    bin_count = Counter([bin_for_n(n) for (n, _, _) in calibration_set])

    print("\nConcluído!")
    print(f"{len(generated)} grafos gerados.")
    print(f"{len(calibration_set)} grafos copiados para {OUT_CALIB}.")

    print("\nDistribuição na calibração por densidade:")
    for p in DENSITIES:
        print(f"  p={p}: {dens_count[p]}")

    print("\nDistribuição na calibração por bins de n:")
    for b in N_BINS:
        print(f"  n={b[0]}..{b[1]}: {bin_count[b]}")

        # calibração estratificado