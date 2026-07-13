# Subtópico: Visão Geral da Abordagem Proposta

**Parágrafo acima (fixo):**
> Este capítulo descreve os métodos, técnicas e procedimentos que foram adotados para o desenvolvimento da proposta de solução para o Problema da Rotulação–L(2,1) em grafos. A seguir, são apresentadas as duas abordagens evolucionárias implementadas — AGBO e AGCAE —, bem como o modelo de PLI utilizado como referência de qualidade. Adicionalmente, são detalhados os procedimentos de calibração automática de parâmetros com o irace, a composição da base de instâncias e o protocolo experimental adotado para a análise comparativa dos resultados.

---

## Versão 1 — Estrutura em etapas sequenciais

### Visão Geral da Abordagem Proposta

A metodologia adotada neste trabalho é organizada em quatro etapas sequenciais, conforme ilustrado na Figura~\ref{fig:fluxo-metodologia}. Na primeira etapa, são coletadas e preparadas as instâncias de grafos que compõem a base experimental, abrangendo grafos estruturados, aleatórios e instâncias de benchmarks consolidados na literatura. Na segunda etapa, os algoritmos propostos — AGBO e AGCAE — são projetados, implementados e validados, juntamente com o modelo de PLI utilizado como referência de otimalidade. Na terceira etapa, a ferramenta irace é empregada para a calibração automática dos parâmetros dos algoritmos evolucionários em um subconjunto representativo de instâncias. Por fim, na quarta etapa, os algoritmos são executados sobre toda a base de grafos e os resultados são analisados de forma comparativa, com base em métricas de qualidade, tempo de execução e estabilidade, utilizando testes estatísticos apropriados.

\begin{figure}[h]
    \centering
    \caption{Fluxograma da metodologia.}
    \includegraphics[width=\textwidth]{figuras/fluxo metodologia.png}
    \caption*{Fonte: Elaborado pelo autor.}
    \label{fig:fluxo-metodologia}
\end{figure}

---

## Versão 2 — Foco nas três abordagens e suas relações

### Visão Geral da Abordagem Proposta

Esta pesquisa propõe a implementação e avaliação comparativa de três abordagens para o Problema da Rotulação–L(2,1): o AGBO, o AGCAE e um modelo de PLI. O modelo de PLI fornece soluções exatas para instâncias de pequeno porte, servindo como referencial de qualidade para as abordagens heurísticas. Já o AGBO e o AGCAE exploram o espaço de soluções por meio de mecanismos evolucionários distintos, compartilhando a mesma heurística gulosa como função de avaliação, mas diferindo na representação cromossômica e nos operadores genéticos aplicados. A Figura~\ref{fig:fluxo-metodologia} sintetiza o fluxo metodológico adotado, desde a preparação da base de instâncias até a análise estatística dos resultados, passando pela calibração de parâmetros com o irace e pela execução dos experimentos.

\begin{figure}[h]
    \centering
    \caption{Fluxograma da metodologia.}
    \includegraphics[width=\textwidth]{figuras/fluxo metodologia.png}
    \caption*{Fonte: Elaborado pelo autor.}
    \label{fig:fluxo-metodologia}
\end{figure}

---

## Versão 3 — Foco na heurística gulosa como elo central

### Visão Geral da Abordagem Proposta

O elemento central que une as abordagens propostas é a heurística gulosa para Rotulação–L(2,1), que atua como função de avaliação tanto no AGBO quanto no AGCAE. No AGBO, a heurística recebe como entrada uma permutação de vértices representada diretamente pelo cromossomo; no AGCAE, os cromossomos de chaves aleatórias são decodificados em ordenações de vértices antes de serem submetidos à heurística. Em ambos os casos, a qualidade de cada solução é medida pelo valor de \(\lambda(G)\) retornado pela heurística. O modelo de PLI, por sua vez, é aplicado de forma independente sobre um subconjunto de instâncias para fornecer valores ótimos de referência. A Figura~\ref{fig:fluxo-metodologia} apresenta o fluxo geral da metodologia, destacando a interação entre os componentes do estudo.

\begin{figure}[h]
    \centering
    \caption{Fluxograma da metodologia.}
    \includegraphics[width=\textwidth]{figuras/fluxo metodologia.png}
    \caption*{Fonte: Elaborado pelo autor.}
    \label{fig:fluxo-metodologia}
\end{figure}

---

## Versão 4 — Foco na reprodutibilidade e rigor experimental

### Visão Geral da Abordagem Proposta

A metodologia foi estruturada de forma a garantir rigor experimental e reprodutibilidade científica. Conforme ilustrado na Figura~\ref{fig:fluxo-metodologia}, o estudo parte de uma base de instâncias padronizada, composta por grafos de múltiplas classes e tamanhos, sobre a qual todos os algoritmos são avaliados sob as mesmas condições. Os parâmetros do AGBO e do AGCAE são calibrados de forma sistemática por meio do irace, evitando ajustes manuais que comprometam a validade dos resultados. As execuções são repetidas múltiplas vezes para capturar a variabilidade estocástica dos métodos, e os resultados são analisados com testes de hipótese adequados. Essa estrutura permite não apenas comparar os algoritmos entre si, mas também quantificar sua distância em relação ao valor ótimo fornecido pelo modelo de PLI.

\begin{figure}[h]
    \centering
    \caption{Fluxograma da metodologia.}
    \includegraphics[width=\textwidth]{figuras/fluxo metodologia.png}
    \caption*{Fonte: Elaborado pelo autor.}
    \label{fig:fluxo-metodologia}
\end{figure}

---

## Versão 5 — Direta e concisa

### Visão Geral da Abordagem Proposta

A Figura~\ref{fig:fluxo-metodologia} apresenta o fluxo geral da metodologia adotada neste trabalho. A base experimental é composta por instâncias de grafos de diferentes classes, tamanhos e densidades, descritas em detalhes na Seção~\ref{sec:base-instancias}. Sobre essa base, são executadas três abordagens: o AGBO e o AGCAE, calibrados automaticamente via irace, e o modelo de PLI, aplicado como referencial de otimalidade em instâncias de pequeno porte. Os resultados de cada abordagem são então analisados comparativamente em termos de qualidade da solução, tempo de execução e estabilidade, conforme o protocolo experimental descrito na Seção~\ref{sec:protocolo-experimental}.

\begin{figure}[h]
    \centering
    \caption{Fluxograma da metodologia.}
    \includegraphics[width=\textwidth]{figuras/fluxo metodologia.png}
    \caption*{Fonte: Elaborado pelo autor.}
    \label{fig:fluxo-metodologia}
\end{figure}
