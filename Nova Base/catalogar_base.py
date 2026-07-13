"""
Gera catalogo_base.csv com métricas de todas as instâncias da Nova Base.

Colunas: file, origin, n, m, density, max_degree
"""

import csv
from pathlib import Path

BASE       = Path("/Users/mateuspedrosa/Documents/TCC-II/Nova Base")
OUTPUT_CSV = BASE / "catalogo_base.csv"


def read_mtx(path: Path) -> tuple[int, int, int]:
    """Retorna (n_vertices, n_edges_reais, max_degree) lendo o MTX inteiro.

    Assume arestas duplicadas (u,v) e (v,u): m_header = 2 × m_real.
    Conta grau apenas para linhas com u < v para evitar dupla contagem.
    """
    with open(path) as f:
        lines = [l for l in f if not l.startswith('%')]

    n, _, m_header = lines[0].split()[:3]
    n, m_header = int(n), int(m_header)
    m_real = m_header // 2

    degree = [0] * (n + 1)
    for line in lines[1:]:
        parts = line.split()
        if len(parts) < 2:
            continue
        u, v = int(parts[0]), int(parts[1])
        if u < v:   # conta cada aresta uma só vez
            degree[u] += 1
            degree[v] += 1

    max_deg = max(degree) if degree else 0
    return n, m_real, max_deg


def density(n: int, m: int) -> float:
    """m deve ser o número real de arestas (sem duplicatas)."""
    return (2 * m) / (n * (n - 1)) if n > 1 else 0.0


def main():
    fields = ["file", "origin", "n", "m", "density", "max_degree"]
    rows = []

    for mtx in sorted(BASE.rglob("*.mtx")):
        try:
            n, m, max_deg = read_mtx(mtx)
        except Exception as e:
            print(f"[AVISO] {mtx.name}: {e}")
            continue

        rows.append({
            "file":       mtx.name,
            "origin":     mtx.parent.name,
            "n":          n,
            "m":          m,
            "density":    round(density(n, m), 6),
            "max_degree": max_deg,
        })

    rows.sort(key=lambda r: (r["origin"], r["n"]))

    with open(OUTPUT_CSV, "w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=fields)
        w.writeheader()
        w.writerows(rows)

    print(f"{len(rows)} instâncias catalogadas → {OUTPUT_CSV}")


if __name__ == "__main__":
    main()
