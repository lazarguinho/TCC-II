"""
Modelo PLI de Shao & Vessel para Rotulação–L(2,1).

Variáveis binárias x[v][i] = 1 se vértice v recebe rótulo i.

Referência:
  Shao, Z. & Vesel, A. (2013). A note on integer linear programming formulations
  for linear ordering problems in graphs. Mathematical Problems in Engineering.

Formulação (caso particular L(2,1), k1=2, k2=1):

  min  z
  s.t.
    sum_{i=0}^{k} x[v][i]  = 1            para todo v
    x[v][i] + x[u][j]     <= 1            para todo {v,u} dist-1, |i-j| <= 1
    x[v][i] + x[u][i]     <= 1            para todo {v,u} dist-2
    i * x[v][i]            <= z            para todo v, i
    x[v][i] in {0,1}
"""

from docplex.mp.model import Model
import networkx as nx
from scipy.io import mmread
from pathlib import Path
import csv
import time
from tqdm import tqdm

input_directory_name = "amostra_60"
pasta_entrada = Path(input_directory_name)

pasta_saida = Path("results_shao_" + input_directory_name)
pasta_saida.mkdir(exist_ok=True)

pasta_saida_rotulacao = Path("labelings_shao_" + input_directory_name)
pasta_saida_rotulacao.mkdir(exist_ok=True)

CSV_AGREGADO = pasta_saida / "_resultados_shao.csv"

TIME_LIMIT_MINUTES = 15


# ── Utilitários ────────────────────────────────────────────────────────────────

def _neighbors_at_distance_1(graph: nx.Graph):
    return {u: set(graph.neighbors(u)) for u in graph.nodes()}


def _neighbors_at_distance_2_fast(graph: nx.Graph, dist1):
    dist2 = {}
    for v in graph.nodes():
        Nv = dist1[v]
        s = set()
        for u in Nv:
            s |= dist1[u]
        s.discard(v)
        s -= Nv
        dist2[v] = s
    return dist2


def check_L21(G, f):
    for u, v in G.edges():
        if abs(f[u] - f[v]) < 2:
            return False
    for u in G.nodes():
        Nu = set(G.neighbors(u))
        for w in Nu:
            for v in G.neighbors(w):
                if v != u and v not in Nu:
                    if abs(f[u] - f[v]) < 1:
                        return False
    return True


def greedy_labeling(graph, order, ub=None):
    f = {i: -1 for i in order}

    if ub is None:
        delta = max(dict(graph.degree()).values(), default=0)
        ub = delta * delta + delta

    possible = list(range(0, ub + 1))
    k = 0

    for i in order:
        forbidden = set()
        for neighbor in graph.neighbors(i):
            ln = f[neighbor]
            if ln != -1:
                forbidden.add(ln)
                forbidden.add(ln - 1)
                forbidden.add(ln + 1)
            for nn in graph.neighbors(neighbor):
                lnn = f[nn]
                if lnn != -1:
                    forbidden.add(lnn)

        chosen = None
        for x in possible:
            if x not in forbidden:
                chosen = x
                break

        if chosen is None:
            start = possible[-1] + 1
            end = start + 200
            possible.extend(range(start, end + 1))
            for x in range(start, end + 1):
                if x not in forbidden:
                    chosen = x
                    break

        if chosen is None:
            raise RuntimeError("Greedy não encontrou rótulo mesmo após expandir.")

        f[i] = chosen
        k = max(k, chosen)

    return k, f


# ── Modelo principal ───────────────────────────────────────────────────────────

def processar_arquivo(arquivo_path: Path):
    arquivo_path = Path(arquivo_path)

    A = mmread(arquivo_path)
    G = nx.from_scipy_sparse_array(A)
    G.remove_edges_from([(u, v) for u, v in G.edges() if u == v])

    n             = G.number_of_nodes()
    edge_count    = G.number_of_edges()
    density       = (2.0 * edge_count) / (n * (n - 1)) if n > 1 else 0.0
    max_degree    = max(dict(G.degree()).values(), default=0)
    min_degree    = min(dict(G.degree()).values(), default=0)
    nodes         = sorted(G.nodes())

    # ── Greedy (upper bound e MIP start) ──────────────────────────────────────
    order     = sorted(nodes, key=lambda v: G.degree(v), reverse=True)
    ub_greedy = max_degree ** 2 + max_degree

    t0 = time.time()
    k0, f0 = greedy_labeling(G, order, ub=ub_greedy)
    greedy_time = time.time() - t0
    greedy_ok = check_L21(G, f0)
    print(f"greedy: {greedy_time:.2f}s | k0={k0} | ok={greedy_ok}")

    # k: limite superior dos rótulos (usa greedy se válido, senão bound teórico)
    k = k0 if greedy_ok else ub_greedy

    # ── Vizinhos ──────────────────────────────────────────────────────────────
    t0 = time.time()
    dist1 = _neighbors_at_distance_1(G)
    dist2 = _neighbors_at_distance_2_fast(G, dist1)
    print(f"dist: {time.time() - t0:.2f}s")

    # ── Modelo Shao & Vessel ──────────────────────────────────────────────────
    mdl = Model(name=f"ShaoVessel_L21_{arquivo_path.stem}")

    # x[v][i] = 1 se vértice v recebe rótulo i   (0 <= i <= k)
    x = {v: mdl.binary_var_list(k + 1, name=f"x{v}_") for v in nodes}
    z = mdl.integer_var(lb=0, name="z")

    # 1) Cada vértice recebe exatamente um rótulo
    for v in nodes:
        mdl.add_constraint(mdl.sum(x[v]) == 1, ctname=f"assign_{v}")

    # 2a) Distância 1: |i - j| <= 1  →  x[v][i] + x[u][j] <= 1
    #     Para cada aresta {v,u} e cada par (i,j) com |i-j| <= 1
    for v, u in G.edges():
        for i in range(k + 1):
            # j = i  (mesmos rótulos proibidos)
            mdl.add_constraint(x[v][i] + x[u][i] <= 1, ctname=f"d1_eq_{v}_{u}_{i}")
            # j = i+1 e j = i-1
            if i + 1 <= k:
                mdl.add_constraint(x[v][i] + x[u][i + 1] <= 1, ctname=f"d1_p_{v}_{u}_{i}")
                mdl.add_constraint(x[v][i + 1] + x[u][i] <= 1, ctname=f"d1_m_{v}_{u}_{i}")

    # 2b) Distância 2: |i - j| < 1  →  i = j  →  x[v][i] + x[u][i] <= 1
    for v in nodes:
        for u in dist2[v]:
            if v < u:
                for i in range(k + 1):
                    mdl.add_constraint(x[v][i] + x[u][i] <= 1, ctname=f"d2_{v}_{u}_{i}")

    # 3) i * x[v][i] <= z   (z >= maior rótulo utilizado)
    for v in nodes:
        for i in range(1, k + 1):          # i=0 é sempre 0*x <= z, trivial
            mdl.add_constraint(i * x[v][i] <= z, ctname=f"ub_{v}_{i}")

    mdl.minimize(z)

    # ── Limite superior de z vindo do greedy ──────────────────────────────────
    if greedy_ok:
        mdl.add_constraint(z <= k0, ctname="ub_from_greedy")

    # ── MIP start ─────────────────────────────────────────────────────────────
    if greedy_ok:
        s = mdl.new_solution()
        s.add_var_value(z, int(k0))
        for v in nodes:
            for i in range(k + 1):
                s.add_var_value(x[v][i], 1 if f0[v] == i else 0)
        mdl.add_mip_start(s)
        print("[INFO] MIP start adicionado.")
    else:
        print("[INFO] Greedy inválido → sem MIP start e sem z<=k0.")

    # ── Parâmetros do solver ───────────────────────────────────────────────────
    mdl.parameters.timelimit          = TIME_LIMIT_MINUTES * 60
    mdl.parameters.mip.display        = 2
    mdl.parameters.emphasis.mip       = 1
    mdl.parameters.mip.strategy.heuristicfreq = 10

    # ── Solve ──────────────────────────────────────────────────────────────────
    t0 = time.time()
    print("solving...")
    sol = mdl.solve(log_output=True)
    solve_time = time.time() - t0
    elapsed_ms = solve_time * 1000.0
    print(f"solve: {solve_time:.2f}s | status: {mdl.solve_details.status}")

    best_bound = mdl.solve_details.best_bound
    bb_nodes = mdl.solve_details.nb_nodes_processed

    if sol is None:
        return (n, edge_count, density, max_degree, min_degree, elapsed_ms, -1,
                f"NO_SOLUTION ({mdl.solve_details.status})", best_bound, -1.0, bb_nodes)

    label_status = ("OPTIMAL" if mdl.solve_details.status == "optimal"
                    else f"BEST FOUND ({mdl.solve_details.status})")

    lambda_val = int(sol.get_value(z))
    gap = (lambda_val - best_bound) / lambda_val if lambda_val > 0 else 0.0

    # Recupera rotulação: para cada vértice, encontra i tal que x[v][i] = 1
    labeling = {}
    for v in nodes:
        for i in range(k + 1):
            if sol.get_value(x[v][i]) > 0.5:
                labeling[v] = i
                break

    # Salva rotulação
    nome_csv = pasta_saida_rotulacao / f"{arquivo_path.stem}.csv"
    with open(nome_csv, "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(["vertex", "label"])
        for v in nodes:
            writer.writerow([v, labeling.get(v, -1)])

    return (n, edge_count, density, max_degree, min_degree, elapsed_ms, lambda_val, label_status, best_bound, gap, bb_nodes)


# ── Loop principal ─────────────────────────────────────────────────────────────

CAMPOS = ["graph", "#vertices", "#edges", "density", "max_degree", "min_degree",
          "time(ms)", "lambda", "status", "lb", "gap", "bb_nodes"]

# cria CSV agregado com cabeçalho apenas se ainda não existir
if not CSV_AGREGADO.exists():
    with open(CSV_AGREGADO, "w", newline="") as f:
        csv.writer(f).writerow(CAMPOS)

arquivos_mtx = sorted([a for a in pasta_entrada.iterdir() if a.suffix == ".mtx"])
for arquivo in tqdm(arquivos_mtx, desc="Processando grafos", unit="grafo"):
    try:
        print(f"\nProcessando {arquivo.name}")
        resultado = processar_arquivo(arquivo)

        linha = [
            arquivo.stem,
            resultado[0],
            resultado[1],
            f"{resultado[2]:.5f}",
            resultado[3],
            resultado[4],
            f"{resultado[5]:.2f}",
            resultado[6],
            resultado[7],
            f"{resultado[8]:.4f}" if resultado[8] is not None else "",
            f"{resultado[9]:.6f}" if resultado[9] >= 0 else "",
            resultado[10],
        ]

        nome_csv = pasta_saida / f"{arquivo.stem}.csv"
        with open(nome_csv, "w", newline="") as f:
            writer = csv.writer(f)
            writer.writerow(CAMPOS)
            writer.writerow(linha)

        with open(CSV_AGREGADO, "a", newline="") as f:
            csv.writer(f).writerow(linha)

        print(f"[OK] {arquivo.name} → {nome_csv.name}")
        arquivo.unlink()
        print(f"[CLEANUP] {arquivo.name} removido da pasta de entrada.")
    except Exception as e:
        tqdm.write(f"[ERRO] {arquivo.name}: {e}")
