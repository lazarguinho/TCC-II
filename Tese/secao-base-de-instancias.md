# Seção — Base de Instâncias

> **Nota:** Seção do Capítulo Metodologia. Todo código referenciado encontra-se em
> `BOGA/utils/` e `BOGA/data/alg aux/`.

---

## 4.X Base de Instâncias

A avaliação experimental dos algoritmos propostos requer uma base de instâncias suficientemente diversificada para que os resultados obtidos sejam generalizáveis a diferentes classes estruturais de grafos. Para atender a esse requisito, a base experimental foi construída a partir de três coleções distintas: instâncias provenientes do repositório Network Repository na categoria DIMACS, instâncias da coleção Harwell–Boeing disponibilizadas pela SuiteSparse Matrix Collection, e grafos aleatórios sintéticos gerados segundo o modelo Erdős–Rényi. A seguir, descrevem-se o processo de coleta, o pré-processamento aplicado e a composição final da base.

### 4.X.1 Coleta das Instâncias

As instâncias da categoria DIMACS foram obtidas a partir do repositório Network Repository~\cite{networkrepository}, acessível em \url{https://networkrepository.com/dimacs.php}. A coleta foi realizada de forma automatizada por um script Python que percorre a página de listagem da categoria, identifica os links de download disponíveis nos formatos comprimidos \texttt{.zip} e \texttt{.7z}, e efetua o download de cada arquivo com mecanismo de retentativa em caso de falhas de rede ou limitação de taxa de acesso pelo servidor. Ao total, foram obtidas 73 instâncias, representando grafos originalmente propostos como benchmarks para problemas de coloração, clique máxima e satisfatibilidade — contextos estruturalmente relevantes para o Problema da Rotulação–L(2,1), dada a semelhança das restrições impostas.

As instâncias da coleção Harwell–Boeing foram baixadas da SuiteSparse Matrix Collection~\cite{suitesparse}, disponível em \url{https://sparse.tamu.edu/HB}. Por utilizar links de download gerados dinamicamente, a coleta exigiu um procedimento em duas etapas: primeiro, o script Python acessa o índice completo da coleção e extrai os identificadores de todas as matrizes disponíveis; em seguida, para cada matriz, a página individual é visitada para localizar o link de download no formato Matrix Market (\texttt{.mm}), que é então obtido com até três tentativas automáticas em caso de falha. O processo de download foi paralelizado com até quatro \textit{threads} simultâneas, e um intervalo de espera entre requisições foi respeitado como medida de cortesia ao servidor. Ao todo, foram coletadas 270 instâncias, correspondendo a grafos de origem estrutural diversa, incluindo matrizes de engenharia, problemas de elementos finitos e sistemas lineares esparsos.

Os grafos aleatórios sintéticos foram gerados programaticamente segundo o modelo Erdős–Rényi $G(n, p)$, utilizando a biblioteca NetworkX~\cite{networkx}. A geração foi controlada por uma semente fixa (\texttt{SEED = 42}) para garantir reprodutibilidade. Foram produzidos 100 grafos distribuídos igualmente entre quatro valores de probabilidade de aresta: $p \in \{0{,}2;\, 0{,}4;\, 0{,}6;\, 0{,}8\}$, com aproximadamente 25 instâncias por nível de densidade. O número de vértices $n$ de cada grafo foi amostrado uniformemente no intervalo $[100, 1000]$, cobrindo assim instâncias de pequeno a médio porte. Cada grafo foi exportado no formato Matrix Market utilizando a biblioteca SciPy~\cite{scipy}, assegurando compatibilidade com o pipeline de pré-processamento e com os algoritmos implementados.

### 4.X.2 Pré-processamento

Antes de serem incorporados à base experimental, todos os grafos — independentemente de sua origem — foram submetidos a um processo uniforme de pré-processamento, implementado no script \texttt{graph\_cleaning.py}. O procedimento adota as seguintes etapas sequenciais:

\begin{enumerate}
    \item \textbf{Leitura}: o arquivo \texttt{.mtx} é lido no formato Matrix Market por meio da função \texttt{mmread} da SciPy, e a matriz resultante é convertida para o formato CSR (\textit{Compressed Sparse Row}) para manipulação eficiente.
    \item \textbf{Validação dimensional}: matrizes não quadradas são descartadas com registro em arquivo de log, pois não correspondem a grafos válidos para o modelo adotado.
    \item \textbf{Construção do grafo}: a matriz esparsa é convertida em um grafo NetworkX por meio de \texttt{nx.from\_scipy\_sparse\_array}. Caso o grafo resultante seja um multigrafo, ele é convertido para grafo simples por \texttt{nx.Graph(G)}, descartando arestas paralelas.
    \item \textbf{Remoção de laços}: laços automáticos (\textit{self-loops}) são removidos com \texttt{nx.selfloop\_edges}, pois o modelo de Rotulação–L(2,1) é definido para grafos simples.
    \item \textbf{Rerotulação}: os vértices são reindexados para o intervalo contíguo $\{0, 1, \ldots, n-1\}$ por meio de um mapeamento determinístico baseado na ordenação dos rótulos originais, garantindo compatibilidade com a representação de listas de adjacência utilizada pelos algoritmos.
    \item \textbf{Exportação}: o grafo pré-processado é serializado novamente no formato Matrix Market com o sufixo \texttt{\_clean.mtx}, preservando o nome original do arquivo.
\end{enumerate}

Todo o processo é registrado em um arquivo de log com marcação temporal, permitindo rastreabilidade de eventuais erros de processamento. O pré-processamento foi aplicado separadamente a cada coleção, gerando três subdiretórios distintos: \texttt{NR\_DIMACS\_cleaned/}, \texttt{Harwell-Boeing\_cleaned/} e \texttt{random/}.

### 4.X.3 Composição Final da Base

Após o pré-processamento, a base experimental completa é composta por 443 instâncias, distribuídas entre as três coleções conforme descrito no Quadro~\ref{qua:base-instancias}.

\begin{quadro}[h]
\centering
\caption{Composição da base de instâncias}
\label{qua:base-instancias}
\begin{tabular}{|l|c|l|}
\hline
\textbf{Coleção} & \textbf{Instâncias} & \textbf{Características} \\
\hline
NR\_DIMACS & 73 & Benchmarks de coloração e clique; grafos densos \\
Harwell–Boeing & 270 & Matrizes de engenharia e elementos finitos; grafos esparsos \\
Erdős–Rényi & 100 & Grafos sintéticos; $n \in [100, 1000]$; $p \in \{0{,}2; 0{,}4; 0{,}6; 0{,}8\}$ \\
\hline
\textbf{Total} & \textbf{443} & \\
\hline
\end{tabular}
\caption*{Fonte: Elaborado pelo autor.}
\end{quadro}

A diversidade estrutural entre as coleções é um aspecto deliberado do projeto experimental. Os grafos DIMACS tendem a ser densos e de tamanho moderado, representando cenários em que o problema de rotulação é computacionalmente mais desafiador. Os grafos Harwell–Boeing, de origem matricial, apresentam estrutura esparsa e variada, com padrões de conectividade distintos dos grafos de benchmark tradicionais. Os grafos Erdős–Rényi, por sua vez, oferecem controle preciso sobre a densidade e o tamanho, permitindo análises parametrizadas do desempenho dos algoritmos em função dessas propriedades.

### 4.X.4 Subconjunto de Calibração

A calibração automática dos parâmetros dos algoritmos evolucionários via irace requer um subconjunto representativo da base completa, utilizado exclusivamente durante a fase de ajuste. Dois subconjuntos de calibração foram definidos:

O primeiro, denominado \texttt{calibration/}, contém 25 instâncias selecionadas a partir dos grafos Erdős–Rényi por amostragem estratificada. A estratificação foi realizada simultaneamente por nível de densidade ($p \in \{0{,}2;\, 0{,}4;\, 0{,}6;\, 0{,}8\}$) e por faixa de tamanho ($n \in [100,250]$, $[251,500]$, $[501,750]$, $[751,1000]$), garantindo representatividade proporcional em ambas as dimensões. Dentro de cada célula do cruzamento densidade $\times$ faixa, as instâncias foram amostradas sem reposição de forma aleatória controlada pela mesma semente (\texttt{SEED = 42}) utilizada na geração dos grafos.

O segundo subconjunto, denominado \texttt{irace\_training/}, contém 140 instâncias selecionadas manualmente para cobrir as três coleções da base, abrangendo grafos de diferentes tamanhos, densidades e origens estruturais. Esse conjunto foi utilizado nas execuções do irace para a calibração dos parâmetros do AGBO e do AGCAE, conforme descrito na Seção~\ref{sec:calibracao}.

A separação entre o conjunto de calibração e o conjunto de avaliação final é essencial para garantir a validade dos resultados: os parâmetros calibrados sobre o subconjunto de treinamento são em seguida aplicados sem ajustes sobre toda a base, evitando o sobreajuste (\textit{overfitting}) das configurações às instâncias de calibração.
