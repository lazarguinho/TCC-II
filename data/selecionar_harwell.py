"""
Seleciona 73 grafos de Harwell-Boeing_cleaned por amostragem estratificada
(grade 4×4: quartis de #vértices × quartis de densidade).
Copia os selecionados para Nova Base/Harwell-Being.
"""

import math
import shutil
from pathlib import Path
import pandas as pd

SRC_DIR  = Path("/Users/mateuspedrosa/Documents/TCC-II/data/Harwell-Boeing_cleaned")
DST_DIR  = Path("/Users/mateuspedrosa/Documents/TCC-II/Nova Base/Harwell-Being")
TARGET_N = 73
N_BINS   = 4
SEED     = 42

DST_DIR.mkdir(parents=True, exist_ok=True)


def parse_mtx_header(path: Path) -> dict:
    with open(path) as f:
        for line in f:
            if line.startswith("%"):
                continue
            parts = line.split()
            n, _, nnz = int(parts[0]), int(parts[1]), int(parts[2])
            break
    edges   = nnz // 2
    density = (2 * edges) / (n * (n - 1)) if n > 1 else 0.0
    return {"file": path.name, "#vertices": n, "#edges": edges,
            "density": round(density, 6)}


# Lê metadados
records = [parse_mtx_header(p) for p in sorted(SRC_DIR.glob("*.mtx"))]
data = pd.DataFrame(records)
total = len(data)
print(f"Total de grafos: {total}")

# Estratificação por quartis de #vértices e densidade
data["bin_v"] = pd.qcut(data["#vertices"], q=N_BINS, labels=False, duplicates="drop")
data["bin_d"] = pd.qcut(data["density"],   q=N_BINS, labels=False, duplicates="drop")

TARGET_FRAC = TARGET_N / total


def sample_stratum(group: pd.DataFrame) -> pd.DataFrame:
    n = max(1, math.ceil(len(group) * TARGET_FRAC))
    n = min(n, len(group))
    return group.sample(n=n, random_state=SEED)


sampled = []
for _, group in data.groupby(["bin_v", "bin_d"], observed=True):
    sampled.append(sample_stratum(group))

selected = pd.concat(sampled, ignore_index=True)

# Ajusta para exatamente TARGET_N se necessário
if len(selected) > TARGET_N:
    selected = selected.sample(n=TARGET_N, random_state=SEED)
elif len(selected) < TARGET_N:
    remaining = data[~data["file"].isin(selected["file"])]
    extra = remaining.sample(n=TARGET_N - len(selected), random_state=SEED)
    selected = pd.concat([selected, extra], ignore_index=True)

print(f"Selecionados: {len(selected)} grafos")

# Relatório por estrato
bin_labels = {0: "Q1", 1: "Q2", 2: "Q3", 3: "Q4"}
selected["bin_v_label"] = selected["bin_v"].map(bin_labels)
selected["bin_d_label"] = selected["bin_d"].map(bin_labels)
summary = selected.groupby(["bin_v_label", "bin_d_label"], observed=True).size()
print("\nDistribuição por estrato (vértices × densidade):")
print(summary.to_string())

# Copia arquivos
for fname in selected["file"]:
    shutil.copy(SRC_DIR / fname, DST_DIR / fname)

print(f"\nArquivos copiados para: {DST_DIR}")

# Salva lista
selected.drop(columns=["bin_v", "bin_d"], errors="ignore").to_csv(
    DST_DIR / "instancias_selecionadas.csv", index=False
)
print(f"Lista salva em: {DST_DIR / 'instancias_selecionadas.csv'}")
