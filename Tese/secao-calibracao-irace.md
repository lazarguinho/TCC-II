# Seção — Calibração de Parâmetros com irace

---

## 4.X Calibração de Parâmetros com irace

A eficácia dos algoritmos evolucionários é fortemente influenciada pela escolha de seus parâmetros de configuração. Valores inadequados podem levar à convergência prematura, à exploração insuficiente do espaço de busca ou ao desperdício de tempo computacional. Para evitar o ajuste manual — que tende a ser enviesado e dificilmente generalizável — adotou-se a ferramenta irace (\textit{Iterated Racing for Automatic Algorithm Configuration})~\cite{irace} para a calibração automática e sistemática dos parâmetros tanto do AGBO quanto do AGCAE.

O irace implementa uma abordagem baseada em corridas estatísticas iterativas (\textit{iterated racing}): em cada iteração, um conjunto de configurações candidatas é amostrado a partir de distribuições probabilísticas sobre o espaço de parâmetros, e as configurações são avaliadas de forma incremental sobre instâncias de treinamento sorteadas aleatoriamente. Configurações com desempenho estatisticamente inferior são eliminadas por meio de testes de hipótese (teste de Friedman por padrão), enquanto as configurações sobreviventes orientam a atualização das distribuições para a iteração seguinte. O processo se repete até que o orçamento de experimentos seja esgotado, retornando um conjunto de configurações elite ordenadas por desempenho acumulado.

### 4.X.1 Conjunto de Instâncias de Treinamento

Ambos os algoritmos foram calibrados sobre o mesmo conjunto de 140 instâncias de treinamento, armazenadas no diretório \texttt{data/irace\_training/}. Esse conjunto foi composto de forma a abranger as três coleções que integram a base experimental — NR\_DIMACS, Harwell–Boeing e grafos Erdős–Rényi —, garantindo diversidade estrutural em termos de tamanho, densidade e origem dos grafos. A adoção de um conjunto de treinamento comum a ambos os algoritmos assegura que as configurações calibradas sejam comparáveis entre si, uma vez que os parâmetros foram ajustados sob as mesmas condições experimentais.

### 4.X.2 Função de Custo

O irace requer que cada execução do algoritmo sobre uma instância retorne um único valor numérico de custo a ser minimizado. Optou-se por uma função de custo composta que penaliza simultaneamente a qualidade da solução e o tempo de execução:

$$\text{custo} = \lambda_f + \alpha \cdot t$$

\noindent onde $\lambda_f$ é o span da melhor rotulação–L(2,1) encontrada ao final da execução, $t$ é o tempo total de execução em segundos, e $\alpha = 0{,}001$ é um fator de ponderação que atribui peso secundário ao tempo, favorecendo primariamente a qualidade da solução. A escolha de $\alpha$ reflete o equilíbrio desejado: soluções de melhor qualidade são sempre preferíveis, mas entre configurações de qualidade equivalente, aquelas que convergem mais rapidamente recebem custo ligeiramente menor.

No AGBO, quando nenhum indivíduo é avaliado com sucesso dentro do limite de tempo de 15 minutos — situação que ocorre em grafos de alta densidade cujo custo de avaliação individual excede o prazo disponível — o valor reportado é uma penalidade igual a $2n$, sempre superior a qualquer span válido para um grafo de $n$ vértices. Isso garante que o irace possa processar o resultado de qualquer instância sem interrupção, tratando configurações que falham sistematicamente em instâncias pesadas como candidatas de baixa aptidão.

### 4.X.3 Configuração do AGBO

Os parâmetros calibrados pelo irace para o AGBO e seus respectivos espaços de busca são descritos no Quadro~\ref{qua:params-agbo}.

\begin{quadro}[h]
\centering
\caption{Parâmetros calibrados pelo irace para o AGBO}
\label{qua:params-agbo}
\begin{tabular}{|l|c|l|l|}
\hline
\textbf{Parâmetro} & \textbf{Flag} & \textbf{Tipo} & \textbf{Intervalo} \\
\hline
Tamanho da população & \texttt{--pop}       & Categórico & $\{20, 22, 24, \ldots, 200\}$ (pares) \\
Máximo de gerações   & \texttt{--gens}      & Inteiro    & $[50,\; 1000]$ \\
Taxa de mutação      & \texttt{--mut}       & Real       & $[0{,}01,\; 1{,}0]$ \\
Taxa de cruzamento   & \texttt{--cr}        & Real       & $[0{,}3,\; 1{,}0]$ \\
Limite de estagnação & \texttt{--max-stagt} & Inteiro    & $[50,\; 2000]$ \\
\hline
\end{tabular}
\caption*{Fonte: Elaborado pelo autor.}
\end{quadro}

O \textit{target-runner} do AGBO executa o binário com os operadores fixos CX (\textit{Cycle Crossover}) e EM (\textit{Exchange Mutation}), reportando o custo composto ao irace. O orçamento total foi de 180 experimentos executados com 4 processos paralelos, com semente do gerador do irace fixada em 123.

A configuração vencedora identificada foi:

\begin{center}
\texttt{--pop 30 --gens 53 --mut 0.446 --cr 0.5724 --max-stagt 1031}
\end{center}

### 4.X.4 Configuração do AGCAE

Os parâmetros calibrados pelo irace para o AGCAE e seus espaços de busca são descritos no Quadro~\ref{qua:params-agcae}.

\begin{quadro}[h]
\centering
\caption{Parâmetros calibrados pelo irace para o AGCAE}
\label{qua:params-agcae}
\begin{tabular}{|l|c|l|l|}
\hline
\textbf{Parâmetro}              & \textbf{Flag}         & \textbf{Tipo} & \textbf{Intervalo} \\
\hline
Tamanho da população            & \texttt{--p}          & Inteiro & $[50,\; 400]$ \\
Fração de elite                 & \texttt{--pe}         & Real    & $[0{,}05,\; 0{,}40]$ \\
Fração de mutantes              & \texttt{--pm}         & Real    & $[0{,}05,\; 0{,}30]$ \\
Viés do cruzamento ($\rho_e$)   & \texttt{--rhoe}       & Real    & $[0{,}60,\; 0{,}90]$ \\
Intervalo de troca de elite     & \texttt{--X\_INTVL}   & Inteiro & $[100,\; 200]$ \\
Número de elites trocados       & \texttt{--X\_NUMBER}  & Inteiro & $[1,\; 8]$ \\
Máximo de gerações              & \texttt{--MAX\_GENS}  & Inteiro & $[300,\; 5000]$ \\
Limite de estagnação            & \texttt{--MAX\_STAGT} & Inteiro & $[50,\; 2000]$ \\
\hline
\end{tabular}
\caption*{Fonte: Elaborado pelo autor.}
\end{quadro}

O \textit{target-runner} do AGCAE executa o binário diretamente com os parâmetros fornecidos pelo irace, reportando o custo composto $\lambda_f + \alpha \cdot t$. O orçamento total foi de 300 experimentos executados com 4 processos paralelos, com semente fixada em 123 — orçamento superior ao do AGBO em razão do maior número de parâmetros a calibrar.

### 4.X.5 Considerações sobre a Validade da Calibração

A separação entre o conjunto de instâncias de treinamento (140 instâncias do \texttt{irace\_training/}) e o conjunto de avaliação final (base completa de 443 instâncias) é essencial para garantir a validade estatística dos experimentos. Os parâmetros calibrados pelo irace são aplicados sem ajustes sobre as instâncias de avaliação, evitando sobreajuste às condições específicas do conjunto de treinamento. Além disso, a natureza não determinística dos algoritmos evolucionários é contemplada pelo irace por meio da configuração \texttt{deterministic = 0}, que permite que o mesmo par (configuração, instância) seja avaliado múltiplas vezes ao longo do processo de calibração, tornando a comparação entre configurações robusta à variabilidade estocástica.
