# Comparação entre os Modelos PLI

## 1. Dimensões de comparação

| Dimensão | Métrica | O que revela |
|---|---|---|
| **Qualidade da solução** | λ(G) encontrado | Se um modelo encontra soluções melhores no mesmo tempo |
| **Otimalidade** | % de instâncias resolvidas até o ótimo dentro do limite | Qual modelo converge mais frequentemente |
| **Tempo até o ótimo** | Tempo (ms) quando ambos chegam ao ótimo | Eficiência da formulação |
| **Gap de otimalidade** | (melhor_encontrado − LB) / melhor_encontrado | Quão longe do ótimo cada modelo fica quando o tempo esgota |
| **Complexidade do B&B** | Nº de nós explorados (disponível no log CPLEX) | Qualidade da relaxação LP de cada formulação |
| **Escalabilidade** | Como tempo e gap crescem com n e Δ | Limites práticos de cada modelo |

---

## 2. Protocolo experimental

**Execução pareada:** o mesmo grafo, com o mesmo time limit (15 min), nos dois modelos. Isso elimina variabilidade de instância e permite testes estatísticos pareados (Wilcoxon signed-rank).

**Estratificação:** os grafos devem cobrir faixas distintas de tamanho e densidade, pois ambos os modelos têm comportamentos muito diferentes nesse espaço:

```
              Baixa densidade (< 0.3)   Alta densidade (≥ 0.3)
n ≤ 50        Célula A                  Célula B
50 < n ≤ 100  Célula C                  Célula D
100 < n ≤ 200 Célula E                  Célula F
```

Grafos muito grandes (n > 300) provavelmente atingem o time limit em ambos — o que é interessante para comparar **melhor solução encontrada**, mas não para comparar **tempo até ótimo**.

---

## 3. Quantos grafos são necessários?

Depende do objetivo:

| Objetivo | Mínimo sugerido | Justificativa |
|---|---|---|
| **Verificação de corretude** (λ conhecido) | 10–15 | Apenas para validar que ambos produzem o mesmo resultado em grafos pequenos |
| **Comparação de desempenho** (tempo, gap) | 30–50 | ~10 por célula da grade 2×3 acima; suficiente para Wilcoxon |
| **Análise de escalabilidade** | 60–100 | Permite ajustar curvas de crescimento por regressão |
| **Publicação** | ≥ 30 por categoria estrutural | Padrão da literatura de PLI para grafos |

**Recomendação prática:** **40–60 instâncias**, estratificadas por faixa de n e densidade, com pelo menos um subconjunto (n ≤ 50, ~15 grafos) onde o ótimo seja alcançável por ambos — para comparar tempo até ótimo. Para as demais, compara-se melhor λ encontrado e gap.

---

## 4. Onde buscar as instâncias

Do que já está disponível na base:

- **NR\_DIMACS pequenos**: `johnson8-2-4`, `MANN-a9`, `hamming6-2`, `hamming6-4` — já têm λ(G) conhecido
- **Harwell-Boeing pequenos**: grafos com n < 100, estrutura esparsa
- **Erdős-Rényi sintéticos**: controle preciso de n e densidade — ideal para análise de escalabilidade

A base do `PLI/select_subset.py` (30% estratificado) já entrega algo próximo disso.
