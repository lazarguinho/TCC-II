# Decisão dos Bins de Estratificação

Documento a decisão dos cortes usados em `amostra_estratificada.py` para a
amostragem estratificada de 60 instâncias da Nova Base (220 instâncias no total).

A análise foi feita sobre `catalogo_base.csv` (gerado por `catalogar_base.py`).
Todos os valores de `m` e densidade no catálogo já consideram arestas únicas
(sem duplicatas), após correção do `catalogar_base.py`.

---

## Bins de Tamanho (nº de vértices)

### Opções avaliadas

| Cortes | Faixas | Contagens | Avaliação |
|--------|--------|-----------|-----------|
| [50, 200, 600] | 4 bins | 20 / 48 / 67 / 85 | Desequilibrado: `>600` concentra 39% |
| **[50, 200, 500, 1000]** | **5 bins** | **20 / 48 / 60 / 54 / 38** | **Mais balanceado** |
| [100, 300, 700] | 4 bins | 35 / 45 / 69 / 71 | Perde separação dos grafos pequenos |

### Decisão: cortes em [50, 200, 500, 1000]

```
P1_tiny   : n ≤ 50     →  20 instâncias  (9%)
P2_small  : n ≤ 200    →  48 instâncias  (22%)
P3_medium : n ≤ 500    →  60 instâncias  (27%)
P4_large  : n ≤ 1000   →  54 instâncias  (25%)
P5_xlarge : n > 1000   →  38 instâncias  (17%)
```

**Justificativa:**
- Mantém o corte em 50 para isolar os grafos muito pequenos (Multipartidos,
  SUBCELAR6, Fibonacci pequenos), onde L(2,1) é trivial.
- Separar 501–1000 de >1000 isola os grafos grandes da Harwell-Boeing
  (alguns chegam a 35 588 vértices) dos grafos de tamanho mediano-grande.
- Range máximo entre bins: 22 instâncias — substancialmente mais equilibrado
  que o intervalo de 37 do esquema anterior.

---

## Bins de Densidade

A densidade é calculada como `d = 2m / (n*(n-1))` com `m` = arestas únicas.

### Distribuição observada nos dados

| Percentil | Densidade |
|-----------|-----------|
| p10 | 0.006 |
| p25 | 0.019 |
| p50 | 0.249 |
| p75 | 0.658 |
| p90 | 0.829 |

A distribuição é **bimodal**: muitos grafos esparsos (Harwell-Boeing, CELAR)
e muitos grafos densos (NR_DIMACS, Aleatórios com p alto).

### Opções avaliadas

| Cortes | Faixas | Contagens | Avaliação |
|--------|--------|-----------|-----------|
| [0.05, 0.30] | 3 bins | 81 / 34 / 105 | `>30%` concentra 48% |
| **[0.05, 0.20, 0.50]** | **4 bins** | **81 / 21 / 38 / 80** | **Melhor separação** |
| [0.02, 0.10, 0.40] | 4 bins | 57 / 36 / 30 / 97 | `>40%` ainda domina |
| [0.05, 0.15, 0.35, 0.65] | 5 bins | 81 / 13 / 23 / 47 / 56 | Faixas do meio muito pequenas |

### Decisão: cortes em [0.05, 0.20, 0.50]

```
D1_sparse : d < 0.05   →  81 instâncias  (37%)  — Harwell-Boeing e CELAR
D2_low    : d < 0.20   →  21 instâncias  (10%)  — Fibonacci e Harwell-Boeing médios
D3_medium : d < 0.50   →  38 instâncias  (17%)  — Aleatórios p baixo, NR_DIMACS médios
D4_dense  : d ≥ 0.50   →  80 instâncias  (36%)  — Aleatórios densos, NR_DIMACS, Multipartidos
```

**Justificativa:**
- Corte em 0.05 separa os grafos de engenharia/estruturais (Harwell-Boeing,
  CELAR) dos demais — esses grafos têm estrutura topológica distinta.
- Corte em 0.20 isola uma faixa intermediária que inclui Fibonacci maiores
  e alguns Harwell-Boeing com mais conectividade.
- Corte em 0.50 divide a massa densa em dois grupos: densos "moderados"
  (Aleatórios com p=0.2–0.4, NR_DIMACS menos densos) e muito densos
  (Aleatórios p=0.6–0.8, NR_DIMACS, Multipartidos Completos).
- Alternativa com 3 bins foi descartada porque deixava 48% das instâncias
  num único estrato, prejudicando a representatividade da amostra.
