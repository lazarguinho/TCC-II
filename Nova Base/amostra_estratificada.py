"""
Amostragem estratificada de 60 instâncias da Nova Base.

Estratificação por 3 eixos:
  - Origem   : subpasta dentro de Nova Base
  - Tamanho  : nº de vértices  (P1 ≤50 | P2 51–200 | P3 201–500 | P4 501–1000 | P5 >1000)
  - Densidade: d = 2m / n(n-1) (D1 <5% | D2 5–20% | D3 20–50% | D4 >50%)

Alocação:
  1. Slots por origem: proporcional ao pool, mínimo 1, teto MAX_PER_ORIGIN.
  2. Dentro de cada origem: slots distribuídos entre estratos (tamanho × densidade)
     proporcionalmente ao tamanho de cada estrato, mínimo 1.
  3. Dentro de cada estrato: escolha aleatória com seed fixo.

Saída: amostra_60.csv  (file, origin, n, m, density, size_bin, dens_bin, path)
"""

import random, csv, shutil
from pathlib import Path
from collections import defaultdict

BASE           = Path("/Users/mateuspedrosa/Documents/TCC-II/Nova Base")
OUTPUT_CSV     = BASE / "amostra_60.csv"
TARGET         = 60
MAX_PER_ORIGIN = 20   # teto por origem para evitar dominância de bases grandes
SEED           = 42

TCC            = BASE.parent
PLI_DIRS = [
    TCC / "PLI Nosso",
    TCC / "PLI Shao e Vessel",
]
SAMPLE_SUBDIR  = "amostra_60"   # nome da subpasta criada em cada PLI

# ── Bins ──────────────────────────────────────────────────────────────────────
# Decisão documentada em: Nova Base/decisao_bins.md
def size_bin(n):
    if n <= 50:   return "P1_tiny"
    if n <= 200:  return "P2_small"
    if n <= 500:  return "P3_medium"
    if n <= 1000: return "P4_large"
    return               "P5_xlarge"

def dens_bin(d):
    if d < 0.05: return "D1_sparse"
    if d < 0.20: return "D2_low"
    if d < 0.50: return "D3_medium"
    return              "D4_dense"

# ── Leitura do cabeçalho MTX ──────────────────────────────────────────────────
def read_mtx_metrics(path):
    """Retorna (n_vertices, m_real) onde m_real = m_header // 2 (arestas duplicadas)."""
    with open(path) as f:
        for line in f:
            if line.startswith('%'):
                continue
            parts = line.split()
            n, m_header = int(parts[0]), int(parts[2])
            return n, m_header // 2
    raise ValueError(f"Cabeçalho não encontrado em {path}")

# ── Coleta todas as instâncias ────────────────────────────────────────────────
def collect_instances():
    instances = []
    for mtx in sorted(BASE.rglob("*.mtx")):
        origin = mtx.parent.name
        try:
            n, m = read_mtx_metrics(mtx)
        except Exception as e:
            print(f"[AVISO] {mtx.name}: {e}")
            continue
        d = (2 * m) / (n * (n - 1)) if n > 1 else 0.0
        instances.append({
            "file": mtx.name, "path": str(mtx), "origin": origin,
            "n": n, "m": m, "density": round(d, 6),
            "size_bin": size_bin(n), "dens_bin": dens_bin(d),
        })
    return instances

# ── Alocação proporcional com mínimo 1 e teto opcional ───────────────────────
def allocate_proportional(counts, total, min_per=1, max_per=None):
    keys       = list(counts)
    total_pool = sum(counts.values())
    alloc      = {k: min_per for k in keys}
    remaining  = total - sum(alloc.values())

    if remaining <= 0:
        for i, k in enumerate(keys):
            alloc[k] = 1 if i < total else 0
        return alloc

    weights = {k: counts[k] / total_pool for k in keys}
    extras  = {k: remaining * weights[k] for k in keys}
    for k in keys:
        alloc[k] += int(extras[k])

    # distribui as sobras inteiras pelo maior fracionário
    leftover = remaining - sum(int(extras[k]) for k in keys)
    for k in sorted(keys, key=lambda k: extras[k] - int(extras[k]), reverse=True)[:leftover]:
        alloc[k] += 1

    # aplica teto e redistribui excesso
    if max_per is not None:
        excess = 0
        for k in keys:
            if alloc[k] > max_per:
                excess += alloc[k] - max_per
                alloc[k] = max_per
        for k in sorted(keys, key=lambda k: alloc[k]):
            if not excess:
                break
            add = min(max_per - alloc[k], excess)
            alloc[k] += add
            excess   -= add

    return alloc

# ── Amostragem ────────────────────────────────────────────────────────────────
def sample(instances):
    random.seed(SEED)

    by_origin = defaultdict(list)
    for inst in instances:
        by_origin[inst["origin"]].append(inst)

    origin_alloc = allocate_proportional(
        {o: len(lst) for o, lst in by_origin.items()},
        TARGET, min_per=1, max_per=MAX_PER_ORIGIN
    )

    selected = []
    for origin, slots in origin_alloc.items():
        pool = by_origin[origin]
        if not pool or slots == 0:
            continue

        by_stratum = defaultdict(list)
        for inst in pool:
            by_stratum[(inst["size_bin"], inst["dens_bin"])].append(inst)

        stratum_alloc = allocate_proportional(
            {s: len(lst) for s, lst in by_stratum.items()},
            slots, min_per=1
        )

        for stratum, n_pick in stratum_alloc.items():
            candidates = by_stratum[stratum]
            selected.extend(random.sample(candidates, min(n_pick, len(candidates))))

    return selected

# ── Relatório e CSV ───────────────────────────────────────────────────────────
def report(instances, selected):
    print(f"\nPool total : {len(instances)}  |  Selecionado: {len(selected)}\n")
    print(f"{'Origem':<30} {'n':>6} {'m':>8} {'density':>9}  {'size':>10}  {'dens':>10}  arquivo")
    print("-" * 105)
    prev = None
    for r in sorted(selected, key=lambda x: (x["origin"], x["n"])):
        if r["origin"] != prev:
            print()
            prev = r["origin"]
        print(f"{r['origin']:<30} {r['n']:>6} {r['m']:>8} {r['density']:>9.5f}  "
              f"{r['size_bin']:>10}  {r['dens_bin']:>10}  {r['file']}")

    # resumo por estrato
    from collections import Counter
    pool_cnt = Counter((r["origin"], r["size_bin"], r["dens_bin"]) for r in instances)
    sel_cnt  = Counter((r["origin"], r["size_bin"], r["dens_bin"]) for r in selected)
    print("\n── Resumo por estrato ──")
    print(f"{'Origem':<30} {'Tam':>10} {'Dens':>10}  {'Pool':>5}  {'Sel':>4}")
    print("-" * 65)
    for key in sorted(pool_cnt):
        print(f"{key[0]:<30} {key[1]:>10} {key[2]:>10}  {pool_cnt[key]:>5}  {sel_cnt.get(key,0):>4}")

def write_csv(selected):
    fields = ["file", "origin", "n", "m", "density", "size_bin", "dens_bin", "path"]
    with open(OUTPUT_CSV, "w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=fields)
        w.writeheader()
        for r in sorted(selected, key=lambda x: (x["origin"], x["n"])):
            w.writerow({k: r[k] for k in fields})
    print(f"\nCSV salvo em: {OUTPUT_CSV}")

# ── Cópia das instâncias para os diretórios PLI ───────────────────────────────
def copy_instances(selected):
    for pli_dir in PLI_DIRS:
        dest = pli_dir / SAMPLE_SUBDIR
        if dest.exists():
            shutil.rmtree(dest)
        dest.mkdir(parents=True)

        for inst in sorted(selected, key=lambda x: x["file"]):
            shutil.copy2(inst["path"], dest / inst["file"])

        print(f"  {len(selected)} arquivos → {dest}")

# ── Main ──────────────────────────────────────────────────────────────────────
if __name__ == "__main__":
    instances = collect_instances()
    selected  = sample(instances)
    report(instances, selected)
    write_csv(selected)
    print("\nCopiando instâncias para os diretórios PLI...")
    copy_instances(selected)
