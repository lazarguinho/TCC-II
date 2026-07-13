# Estrutura de Capítulos — Dissertação de Mestrado

Sugestão baseada na análise da Monografia (TCC-II) sobre o Problema de Rotulação–L(2,1) com Algoritmos Genéticos.

---

## Capítulo 1 — Introdução

- 1.1 Contextualização (alocação de frequências, escassez de espectro)
- 1.2 Definição do Problema de Rotulação–L(2,1)
- 1.3 Objetivos Gerais e Específicos
- 1.4 Contribuições do Trabalho
- 1.5 Organização da Dissertação

---

## Capítulo 2 — Fundamentação Teórica

- 2.1 Conceitos de Teoria dos Grafos
  - 2.1.1 Definições básicas (grafos, grau, distância, conectividade)
  - 2.1.2 Classes de grafos relevantes (árvores, cordais, completos, k-partidos, caminhos)
  - 2.1.3 Coloração de vértices e número cromático
- 2.2 Rotulação de Grafos
  - 2.2.1 T-coloração e origens
  - 2.2.2 Rotulação–L(h,k): generalização formal
  - 2.2.3 Rotulação–L(2,1): definição, NP-completude e casos polinomiais conhecidos
  - 2.2.4 Heurística gulosa e Lema da ordenação ótima
  - 2.2.5 Limitantes teóricos: inferior (Δ+1) e superiores (Griggs-Yeh, Gonçalves)
  - 2.2.6 Valores exatos de λ(G) para classes específicas
- 2.3 Programação Linear Inteira
  - 2.3.1 Fundamentos de PLI
  - 2.3.2 Formulação PLI para L(2,1) (Shao et al.)
  - 2.3.3 Estratégias de resolução (branch-and-bound, cutting planes)
- 2.4 Meta-heurísticas Evolutivas
  - 2.4.1 Algoritmos Genéticos clássicos
  - 2.4.2 Algoritmos Genéticos Baseados em Ordem (AGBO)
  - 2.4.3 Algoritmos Genéticos de Chaves Aleatórias Enviesadas (AGCAE/BRKGA)
- 2.5 Calibração Automática de Algoritmos
  - 2.5.1 Problema de configuração de algoritmos
  - 2.5.2 irace: corridas iterativas

---

## Capítulo 3 — Revisão Sistemática da Literatura

- 3.1 Protocolo de Revisão (bases consultadas, critérios de inclusão/exclusão)
- 3.2 Métodos Exatos para o Problema de Rotulação–L(2,1)
- 3.3 Heurísticas e Meta-heurísticas para Rotulação–L(2,1)
  - 3.3.1 Abordagens baseadas em ordenação de vértices
  - 3.3.2 Algoritmos Genéticos (Coreanos, Indianos, Taranenko et al.)
  - 3.3.3 Outras meta-heurísticas (busca tabu, simulated annealing, etc.)
- 3.4 AGCAE/BRKGA em Problemas de Rotulação e Coloração
- 3.5 Síntese: lacunas identificadas e posicionamento desta pesquisa
- 3.6 Quadro Comparativo dos Trabalhos

---

## Capítulo 4 — Metodologia

- 4.1 Visão Geral da Abordagem Proposta
- 4.2 Algoritmo Genético Baseado em Ordem (AGBO)
  - 4.2.1 Representação cromossômica e decodificação
  - 4.2.2 Estratégias de inicialização da população (aleatória, Maior-Primeiro, Menor-Último)
  - 4.2.3 Avaliação de aptidão (heurística gulosa)
  - 4.2.4 Seleção (roleta viciada)
  - 4.2.5 Operadores de cruzamento (CX, OX, PMX)
  - 4.2.6 Operadores de mutação (EM, DM, SM, ISM, SIM)
  - 4.2.7 Elitismo e critério de parada
- 4.3 Algoritmo Genético de Chaves Aleatórias Enviesadas (AGCAE)
  - 4.3.1 Representação por chaves aleatórias
  - 4.3.2 Decodificador para L(2,1)
  - 4.3.3 Estrutura da população (elite, não-elite, mutantes)
  - 4.3.4 Cruzamento uniforme parametrizado
- 4.4 Modelo de Programação Linear Inteira (benchmark)
- 4.5 Base de Instâncias (~400 grafos)
  - 4.5.1 Grafos estruturados (árvores, caminhos, completos, k-partidos)
  - 4.5.2 Grafos aleatórios (Erdős–Rényi)
  - 4.5.3 Instâncias de benchmark (DIMACS, CELLAR, Harwell-Boeing)
- 4.6 Calibração de Parâmetros com irace
- 4.7 Protocolo Experimental e Análise Estatística
  - 4.7.1 Métricas (span, tempo, desvio padrão)
  - 4.7.2 Testes de hipótese (Wilcoxon pareado, Friedman)
  - 4.7.3 Gap de otimalidade em relação ao PLI

---

## Capítulo 5 — Implementação

- 5.1 Arquitetura do Software
- 5.2 Implementação do AGBO (C++)
  - 5.2.1 Estrutura de dados do grafo e leitura de .mtx
  - 5.2.2 Implementação dos operadores genéticos
  - 5.2.3 Validação da implementação
- 5.3 Implementação do AGCAE (C++ com BRKGA API)
  - 5.3.1 Decodificador e integração com a biblioteca
- 5.4 Implementação do Modelo PLI (Python/CPLEX)
- 5.5 Ambiente Computacional e Reprodutibilidade

---

## Capítulo 6 — Experimentos e Resultados

- 6.1 Validação das Implementações (grafos completos K_n e instâncias com λ(G) conhecido)
- 6.2 Resultados da Calibração com irace
  - 6.2.1 Melhores configurações do AGBO
  - 6.2.2 Melhores configurações do AGCAE
- 6.3 Impacto dos Operadores de Cruzamento e Mutação
- 6.4 Impacto das Estratégias de Inicialização da População
- 6.5 AGBO vs. AGCAE: comparação direta
- 6.6 Comparação com o Modelo PLI (gap de otimalidade)
- 6.7 Comparação com Limitantes Superiores Conhecidos
- 6.8 Análise por Classe de Grafo
  - 6.8.1 Árvores e caminhos
  - 6.8.2 Grafos aleatórios
  - 6.8.3 Instâncias DIMACS e CELLAR
- 6.9 Análise de Escalabilidade (tempo vs. tamanho do grafo)
- 6.10 Análise Estatística (Wilcoxon, Friedman, efeito de tamanho)

---

## Capítulo 7 — Discussão

- 7.1 Interpretação dos Resultados por Classe de Grafo
- 7.2 Trade-off entre Qualidade da Solução e Tempo de Execução
- 7.3 Vantagens e Limitações de Cada Abordagem
- 7.4 Relação com os Trabalhos da Literatura

---

## Capítulo 8 — Conclusões e Trabalhos Futuros

- 8.1 Síntese das Contribuições
- 8.2 Limitações do Estudo
- 8.3 Trabalhos Futuros
  - Generalização para L(p,q) com p,q arbitrários
  - Hibridização com busca local
  - Instâncias de maior porte
  - Aplicação a grafos de interferência reais

---

## Principais diferenças em relação ao TCC-II

| Aspecto | TCC-II | Dissertação |
|---|---|---|
| Revisão de literatura | 3 trabalhos pontuais | Revisão sistemática com protocolo formal |
| Teoria | L(2,1) isolado | L(h,k) contextualizado, casos polinomiais |
| Análise estatística | Mencionada na metodologia | Capítulo dedicado com Wilcoxon/Friedman |
| Implementação | Implícita na metodologia | Capítulo próprio com validação |
| Discussão | Embutida nos resultados | Capítulo separado e aprofundado |
| Contribuições | Implícitas | Seção explícita na introdução |
