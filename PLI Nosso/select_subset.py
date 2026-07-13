"""
Seleciona 30% da base de instâncias como subconjunto representativo,
balanceado por número de vértices e densidade via amostragem estratificada
por quartis (grade 4×4 = 16 estratos).

Lê os arquivos .mtx diretamente dos diretórios de dados do BOGA e extrai
#vertices, #edges e density do cabeçalho de cada arquivo.

Saída:
  - subset_30pct.csv         : instâncias selecionadas com metadados
  - subset_30pct_summary.csv : contagem por estrato (antes e depois da seleção)
"""

import math
from pathlib import Path

import pandas as pd

# ── Configuração ─────────────────────────────────────────────────────────────

BOGA_DIR = Path(__file__).parent.parent / "BOGA" / "data"

DATA_DIRS = [
    BOGA_DIR / "Harwell-Boeing_cleaned",
    BOGA_DIR / "NR_DIMACS_cleaned",
    BOGA_DIR / "random",
]

TARGET_FRAC = 0.30
N_BINS = 4
RANDOM_SEED = 42

# ── Leitura dos .mtx ──────────────────────────────────────────────────────────

def parse_mtx_header(path: Path) -> dict:
    """Extrai n, #edges e density do cabeçalho do .mtx."""
    with open(path) as f:
        for line in f:
            if line.startswith("%"):
                continue
            n, _, nnz = (int(x) for x in line.split())
            break
    edges = nnz // 2
    density = (2 * edges) / (n * (n - 1)) if n > 1 else 0.0
    return {"graph": path.stem, "source": path.parent.name,
            "#vertices": n, "#edges": edges, "density": round(density, 5)}

records = []
for d in DATA_DIRS:
    for mtx in sorted(d.glob("*.mtx")):
        records.append(parse_mtx_header(mtx))

data = pd.DataFrame(records)
print(f"Base total: {len(data)} instâncias")
print(data.groupby("source").size().to_string())

# ── Estratificação por quartis ────────────────────────────────────────────────
# pd.qcut com duplicates="drop" lida com empates nas fronteiras dos quantis.

data["bin_vertices"] = pd.qcut(
    data["#vertices"], q=N_BINS, labels=False, duplicates="drop"
)
data["bin_density"] = pd.qcut(
    data["density"], q=N_BINS, labels=False, duplicates="drop"
)

# ── Amostragem estratificada ──────────────────────────────────────────────────
# Em cada estrato seleciona ceil(|estrato| × TARGET_FRAC) instâncias,
# garantindo ao menos 1 por estrato não vazio.

def sample_stratum(group: pd.DataFrame) -> pd.DataFrame:
    n = max(1, math.ceil(len(group) * TARGET_FRAC))
    n = min(n, len(group))
    return group.sample(n=n, random_state=RANDOM_SEED)

sampled_groups = []
for _, group in data.groupby(["bin_vertices", "bin_density"]):
    sampled_groups.append(sample_stratum(group))
selected = pd.concat(sampled_groups, ignore_index=True)

print(f"\nSelecionadas: {len(selected)} ({len(selected) / len(data) * 100:.1f}%)")

# ── Relatório por estrato ─────────────────────────────────────────────────────

summary = (
    data
    .groupby(["bin_vertices", "bin_density"])
    .agg(
        total=("graph", "count"),
        vertices_min=("#vertices", "min"),
        vertices_max=("#vertices", "max"),
        density_min=("density", "min"),
        density_max=("density", "max"),
    )
    .reset_index()
)

selected_counts = (
    selected
    .groupby(["bin_vertices", "bin_density"])
    .size()
    .reset_index(name="selecionadas")
)

summary = summary.merge(selected_counts, on=["bin_vertices", "bin_density"], how="left")
summary["selecionadas"] = summary["selecionadas"].fillna(0).astype(int)
summary["frac_real"] = (summary["selecionadas"] / summary["total"]).round(3)

bin_labels = {0: "Q1", 1: "Q2", 2: "Q3", 3: "Q4"}
summary["bin_vertices"] = summary["bin_vertices"].map(bin_labels)
summary["bin_density"] = summary["bin_density"].map(bin_labels)
selected["bin_vertices"] = selected["bin_vertices"].map(bin_labels)
selected["bin_density"] = selected["bin_density"].map(bin_labels)

print("\nResumo por estrato:")
print(summary.to_string(index=False))

# ── Salvamento ────────────────────────────────────────────────────────────────

out_dir = Path(__file__).parent
out_instances = out_dir / "subset_30pct.csv"
out_summary   = out_dir / "subset_30pct_summary.csv"

selected.to_csv(out_instances, index=False)
summary.to_csv(out_summary, index=False)

print(f"\nArquivos gerados:")
print(f"  {out_instances}")
print(f"  {out_summary}")
