# Seção 4.2 — Algoritmo Genético Baseado em Ordem (AGBO)
## 5 versões | Estrutura: Visão Geral (2p) · Cromossomo (2p) · Pop. Inicial (1p) · Fitness (1p) · Operadores (4p) · Elitismo e Parada (3p)

---

## VERSÃO 1 — Descritiva e direta

### 4.2 Algoritmo Genético Baseado em Ordem (AGBO)

**[Visão Geral — P1]**
O Algoritmo Genético Baseado em Ordem (AGBO) é uma variante dos algoritmos genéticos clássicos projetada para operar sobre representações permutacionais, sendo amplamente utilizada em problemas combinatórios em que a solução pode ser codificada como uma sequência ordenada de elementos. No contexto do Problema da Rotulação–L(2,1), o AGBO não constrói diretamente uma rotulação; em vez disso, evolui uma população de ordenações dos vértices do grafo, utilizando a qualidade da rotulação produzida por uma heurística gulosa — aplicada sobre cada ordenação — como critério de avaliação da aptidão dos indivíduos.

**[Visão Geral — P2]**
Essa abordagem é fundamentada no Lema~\ref{lema:ordenacao-otima-gulosa}, que garante a existência de ao menos uma ordenação dos vértices para a qual a heurística gulosa produz uma rotulação ótima. Assim, em vez de buscar diretamente no espaço exponencial de todas as rotulações válidas, o AGBO concentra a busca no espaço das permutações de vértices, explorando-o por meio de operadores genéticos adaptados à representação permutacional e guiando a evolução rumo a ordenações que induzam rotulações de menor span.

**[Representação do Cromossomo — P1]**
Cada indivíduo da população é representado por um vetor de inteiros de comprimento $n$, onde $n$ é o número de vértices do grafo. Esse vetor corresponde a uma permutação dos identificadores dos vértices $\{0, 1, \ldots, n-1\}$, codificando a ordem em que a heurística gulosa deve processar os vértices ao construir a rotulação. Formalmente, o cromossomo de um indivíduo é uma bijeção $\sigma: \{1, \ldots, n\} \to V(G)$, em que a posição $i$ indica o $i$-ésimo vértice a ser rotulado.

**[Representação do Cromossomo — P2]**
A escolha da representação permutacional impõe restrições importantes aos operadores genéticos: cruzamentos e mutações devem, necessariamente, produzir descendentes que também sejam permutações válidas, ou seja, sequências sem repetição nem ausência de elementos. Essa restrição motivou a adoção de operadores especializados para permutações, que preservam a integridade estrutural das soluções ao longo de todo o processo evolutivo, diferentemente de operadores genéticos convencionais que atuam sobre representações binárias ou inteiras sem tais restrições.

**[Geração da População Inicial — P1]**
A população inicial é gerada de forma aleatória: o primeiro indivíduo corresponde à sequência canônica dos vértices $(0, 1, \ldots, n-1)$, enquanto os demais $p-1$ indivíduos são obtidos por embaralhamentos aleatórios independentes dessa sequência base, utilizando o algoritmo de Fisher-Yates. Essa estratégia garante diversidade inicial na população, cobrindo uniformemente o espaço de permutações, e estabelece um ponto de partida neutro, sem viés heurístico sobre a estrutura do grafo de entrada.

**[Função de Avaliação — P1]**
A aptidão de cada indivíduo é determinada aplicando-se a heurística gulosa descrita no Algoritmo~\ref{alg:greedy} sobre a permutação que ele representa, obtendo-se um valor de span $\lambda_f$. Como o objetivo é minimizar o span, a aptidão é inversamente proporcional a $\lambda_f$: indivíduos com menor span recebem maior probabilidade de seleção. A função de avaliação é, portanto, $\text{fitness}(\sigma) = \lambda_f(\sigma)$, e o algoritmo busca minimizá-la ao longo das gerações.

**[Operadores Genéticos — P1: Seleção]**
A seleção dos pais para reprodução é realizada pelo método da roleta viciada com probabilidades inversas ao fitness. Para cada indivíduo $i$ com fitness $f_i$, o peso de seleção é calculado como $w_i = 1 - f_i / \sum_j f_j$, de modo que indivíduos com menor span recebem maior probabilidade de serem escolhidos. Em situações degeneradas em que todos os pesos se anulam — o que ocorre quando todos os indivíduos possuem fitness idêntico —, a seleção recai sobre uma distribuição uniforme, preservando a aleatoriedade do processo.

**[Operadores Genéticos — P2: Cruzamento]**
O cruzamento é aplicado com probabilidade $p_c$ (taxa de cruzamento) a cada par de pais selecionado. Foram implementados dois operadores: o \textit{Cycle Crossover} (CX) e o \textit{Order Crossover} (OX). O CX identifica ciclos de posições entre os pais e distribui os genes de forma a preservar a posição absoluta de cada elemento na permutação. O OX, por sua vez, copia um segmento contínuo de um dos pais e preenche as posições restantes do filho com os elementos do outro pai, respeitando a ordem relativa em que eles aparecem. Ambos os operadores garantem que os filhos gerados sejam permutações válidas. Quando o cruzamento não é aplicado (com probabilidade $1 - p_c$), os filhos são cópias diretas dos pais.

**[Operadores Genéticos — P3: Mutação por Troca e Deslocamento]**
A mutação é aplicada individualmente a cada descendente gerado com probabilidade $p_m$ (taxa de mutação). Três dos cinco operadores de mutação implementados atuam por reposicionamento de elementos: o \textit{Exchange Mutation} (EM) troca as posições de dois genes escolhidos aleatoriamente; o \textit{Displacement Mutation} (DM) extrai um segmento contínuo da permutação e o reinsere em uma posição aleatória, preservando a ordem interna do segmento; e o \textit{Insertion Mutation} (ISM) remove um único gene de sua posição original e o insere em outra posição da sequência.

**[Operadores Genéticos — P4: Mutação por Inversão e Embaralhamento]**
Os dois operadores restantes atuam por modificação da ordem interna de segmentos: o \textit{Scramble Mutation} (SM) seleciona um segmento contínuo da permutação e embaralha aleatoriamente os elementos dentro desse intervalo, introduzindo alta perturbação local; e o \textit{Simple Inversion Mutation} (SIM) inverte a ordem dos elementos de um segmento contínuo selecionado aleatoriamente, preservando o conjunto de elementos do segmento mas alterando sua sequência. Todos os operadores de mutação produzem permutações válidas por construção, sem necessidade de reparação.

**[Elitismo e Critério de Parada — P1: Elitismo]**
O AGBO adota uma estratégia de elitismo que preserva os dois melhores indivíduos de cada geração, transferindo-os diretamente para a população seguinte antes do processo de reprodução. Essa abordagem garante que a melhor solução encontrada até o momento nunca seja perdida durante a evolução, assegurando a monotonia da qualidade da melhor solução ao longo das gerações. Os demais $p - 2$ indivíduos da nova população são gerados por cruzamento e mutação a partir dos pais selecionados pela roleta viciada.

**[Elitismo e Critério de Parada — P2: MAX\_GENS e Estagnação]**
O algoritmo utiliza dois critérios de parada baseados no progresso da busca. O primeiro limita o número máximo de gerações a $G_{\max}$: ao atingir esse limite, o algoritmo encerra independentemente da qualidade da melhor solução encontrada. O segundo critério detecta estagnação: um contador é incrementado a cada geração em que a melhor solução não melhora, e o algoritmo para quando esse contador atinge o limite $S_{\max}$. Esse mecanismo evita desperdício de tempo computacional em regiões do espaço de busca nas quais o algoritmo não está mais progredindo.

**[Elitismo e Critério de Parada — P3: Limite de Tempo]**
O terceiro critério de parada é um limite de tempo de 15 minutos por execução, implementado por meio de um \textit{deadline} absoluto definido no início de cada run. O prazo é verificado no início do processamento de cada vértice dentro da heurística gulosa, de modo que qualquer avaliação de fitness em andamento pode ser interrompida ao nível de granularidade de um vértice quando o tempo limite é atingido. Caso o prazo expire antes que qualquer indivíduo seja avaliado com sucesso, o algoritmo reporta que não foi possível obter resultado dentro do prazo, evitando execuções indefinidas sobre instâncias de grande porte e alta densidade.

---

## VERSÃO 2 — Técnica e formal

### 4.2 Algoritmo Genético Baseado em Ordem (AGBO)

**[Visão Geral — P1]**
O AGBO é uma meta-heurística evolucionária baseada no paradigma dos algoritmos genéticos clássicos, adaptada para operar sobre o espaço de permutações. A motivação para essa escolha reside no Lema~\ref{lema:ordenacao-otima-gulosa}: como existe ao menos uma permutação dos vértices para a qual a heurística gulosa produz uma rotulação ótima, o problema de encontrar $\lambda(G)$ pode ser reduzido, sem perda de generalidade, ao problema de encontrar a melhor ordenação dos vértices. O AGBO explora esse espaço de permutações por meio de evolução estocástica guiada pela qualidade das rotulações produzidas.

**[Visão Geral — P2]**
O algoritmo mantém uma população de $p$ indivíduos ao longo de $G$ gerações. A cada geração, a população é avaliada pela heurística gulosa, os melhores indivíduos são selecionados para reprodução por meio da roleta viciada, e novos indivíduos são gerados pela aplicação de operadores de cruzamento e mutação especializados para permutações. O ciclo evolutivo continua até que um dos critérios de parada seja satisfeito: número máximo de gerações $G_{\max}$, estagnação por $S_{\max}$ gerações consecutivas sem melhora, ou esgotamento do limite de tempo de 15 minutos por run.

**[Representação do Cromossomo — P1]**
Formalmente, cada indivíduo é representado por uma permutação $\sigma = (\sigma_1, \sigma_2, \ldots, \sigma_n)$ dos vértices $V = \{0, 1, \ldots, n-1\}$ do grafo $G$. A permutação codifica a ordem de processamento dos vértices pela heurística gulosa: o vértice $\sigma_i$ é o $i$-ésimo a receber um rótulo. O espaço de busca do AGBO é, portanto, o conjunto $S_n$ de todas as permutações de $n$ elementos, com cardinalidade $n!$, cujo tamanho cresce de forma super-exponencial com $n$.

**[Representação do Cromossomo — P2]**
A representação permutacional exige que todos os operadores genéticos produzam permutações válidas como saída, isto é, sequências nas quais cada vértice apareça exatamente uma vez. Essa restrição elimina a possibilidade de uso direto de operadores genéticos convencionais — como cruzamento de um ponto ou mutação por inversão de bit —, que frequentemente produzem soluções inviáveis quando aplicados a permutações. Para contornar essa limitação, foram adotados operadores especialmente projetados para o contexto permutacional, que por construção mantêm a viabilidade dos indivíduos gerados.

**[Geração da População Inicial — P1]**
A população inicial é composta por $p$ permutações geradas de forma aleatória e independente, utilizando o gerador Mersenne Twister com semente obtida do dispositivo de aleatoriedade do sistema (\texttt{std::random\_device}), garantindo resultados distintos entre execuções independentes. O primeiro indivíduo corresponde à permutação identidade $(0, 1, \ldots, n-1)$, e os demais são obtidos por embaralhamentos sucessivos dessa sequência, assegurando que a população inicial cubra de forma ampla e diversa o espaço de permutações sem qualquer viés estrutural sobre a topologia do grafo.

**[Função de Avaliação — P1]**
A aptidão de um indivíduo $\sigma$ é definida como o span $\lambda_f(\sigma)$ produzido pela heurística gulosa (Algoritmo~\ref{alg:greedy}) quando aplicada à ordenação $\sigma$. Como o objetivo é minimizar $\lambda_f(\sigma)$, a seleção favorece indivíduos com menores valores de fitness. No pior caso, o custo de avaliação de um único indivíduo é $\mathcal{O}(n \Delta^2)$, onde $\Delta$ é o grau máximo do grafo, pois a heurística deve, para cada vértice, percorrer seus vizinhos e os vizinhos dos vizinhos para calcular o conjunto de rótulos proibidos. Esse custo é o principal fator determinante da complexidade computacional de cada geração do AGBO.

**[Operadores Genéticos — P1: Seleção]**
A seleção de pais para reprodução é realizada pelo método da roleta viciada com pesos inversamente proporcionais ao fitness. Dado o vetor de aptidões $(f_1, \ldots, f_p)$, o peso de seleção do indivíduo $i$ é $w_i = 1 - f_i / \sum_{j=1}^{p} f_j$, de modo que indivíduos com menor span recebem maior probabilidade de participar da reprodução. A distribuição de probabilidade resultante é normalizada e amostrada utilizando \texttt{std::discrete\_distribution}, com fallback para distribuição uniforme em casos degenerados nos quais todos os pesos são nulos.

**[Operadores Genéticos — P2: Cruzamento]**
Com probabilidade $p_c$, um operador de cruzamento é aplicado ao par de pais selecionado, gerando dois filhos. Foram implementados dois operadores: CX e OX. O CX opera identificando ciclos de correspondência entre as posições dos dois pais: a posição $i$ de um filho recebe o gene do Pai 1 se $i$ pertence a um ciclo de índice ímpar, e do Pai 2 caso contrário, garantindo que cada elemento apareça exatamente uma vez em cada filho. O OX, por sua vez, copia um segmento contíguo $[\ell, r]$ do Pai 1 para o filho e preenche as posições restantes com os elementos do Pai 2 na ordem em que aparecem a partir da posição $r+1$ de forma circular, também preservando a viabilidade da permutação resultante.

**[Operadores Genéticos — P3: Mutação por Reposicionamento]**
A mutação é aplicada a cada indivíduo gerado com probabilidade independente $p_m$. O EM troca dois genes selecionados aleatoriamente, introduzindo uma perturbação mínima na permutação. O DM extrai um segmento $[\ell, r]$ e o reinsere em uma posição aleatória do restante da sequência, deslocando os demais genes para preencher o espaço liberado. O ISM é uma versão pontual do DM: extrai um único gene e o reinsere em outra posição. Esses três operadores realizam perturbações locais de magnitude controlada e são adequados para exploração refinada em torno de soluções já conhecidas.

**[Operadores Genéticos — P4: Mutação por Reorganização Interna]**
O SM e o SIM atuam sobre a ordenação interna de um segmento selecionado aleatoriamente. O SM embaralha os elementos do segmento $[\ell, r]$ de forma completamente aleatória, produzindo perturbações de alta intensidade, úteis para escapar de ótimos locais. O SIM inverte a ordem dos elementos no segmento $[\ell, r]$, com $\ell < r$ garantido por construção, introduzindo uma reorganização sistemática e determinística dentro do intervalo selecionado. Todos os operadores implementados preservam, por construção, a propriedade de permutação dos cromossomos resultantes.

**[Elitismo e Critério de Parada — P1]**
A estratégia de elitismo aplicada no AGBO preserva os dois indivíduos de menor span de cada geração, inserindo-os diretamente no início da nova população antes da etapa de reprodução. Como consequência, a melhor solução encontrada ao longo da execução é monotonicamente não-crescente em termos de span: $\lambda_{\min}^{(g+1)} \leq \lambda_{\min}^{(g)}$ para toda geração $g$. Os $p - 2$ indivíduos restantes da nova população são gerados por cruzamento e mutação dos pais selecionados, completando o ciclo evolutivo.

**[Elitismo e Critério de Parada — P2]**
O critério de parada por estagnação monitora o número de gerações consecutivas sem melhora no melhor span encontrado. Um contador de estagnação é incrementado a cada geração em que $\lambda_{\min}^{(g)} = \lambda_{\min}^{(g-1)}$ e zerado sempre que ocorre melhora. Quando o contador atinge $S_{\max}$ (calibrado pelo irace), o algoritmo encerra, evitando iterações improdutivas. O critério de parada por número máximo de gerações $G_{\max}$ funciona como limite superior absoluto, independentemente do progresso da busca.

**[Elitismo e Critério de Parada — P3]**
O terceiro critério de parada é um limite de tempo de 15 minutos por run, implementado por meio de um \textit{deadline} absoluto ($t_0 + 15\text{ min}$) verificado ao início do processamento de cada vértice na heurística gulosa. Essa granularidade de verificação garante que o algoritmo possa ser interrompido no meio de uma avaliação de fitness, caso o prazo seja atingido durante o processamento de um grafo de grande porte. Ao final da execução — por qualquer um dos três critérios —, é reportado o melhor span encontrado ao longo de toda a run, bem como a ordenação e a rotulação correspondentes, registradas no momento em que foram descobertas durante a avaliação de fitness.

---

## VERSÃO 3 — Narrativa e contextualizada

### 4.2 Algoritmo Genético Baseado em Ordem (AGBO)

**[Visão Geral — P1]**
A construção de uma rotulação–L(2,1) de baixo span para um grafo $G$ é um problema intrinsecamente combinatório: o número de configurações possíveis cresce exponencialmente com o tamanho da instância, tornando inviável qualquer busca exaustiva para grafos de porte moderado. Uma alternativa eficaz, explorada neste trabalho, é reformular o problema de otimização em termos de ordenações de vértices: dado que a heurística gulosa produz soluções de qualidade variável conforme a ordem em que os vértices são processados, e que existe ao menos uma ordenação que conduz à solução ótima (Lema~\ref{lema:ordenacao-otima-gulosa}), o desafio se reduz a encontrar boas ordenações dentro do espaço $S_n$ de permutações.

**[Visão Geral — P2]**
É nesse contexto que o AGBO se insere. Ao contrário de algoritmos genéticos convencionais que operam sobre representações binárias ou inteiras, o AGBO mantém e evolui uma população de permutações dos vértices, utilizando operadores genéticos especialmente projetados para preservar a viabilidade das soluções ao longo do processo evolutivo. A aptidão de cada permutação é avaliada pela heurística gulosa, e o processo de seleção, cruzamento e mutação guia a população em direção a ordenações que induzam rotulações de menor span.

**[Representação do Cromossomo — P1]**
No AGBO, cada indivíduo da população é um vetor de $n$ inteiros distintos representando uma permutação dos vértices $\{0, 1, \ldots, n-1\}$ do grafo. Essa permutação define a sequência em que a heurística gulosa processa os vértices: o vértice na primeira posição do vetor recebe seu rótulo primeiro, seguido do vértice na segunda posição, e assim por diante. A qualidade da rotulação resultante depende fortemente dessa ordem, o que torna o problema de encontrar a permutação ótima equivalente, em dificuldade, ao próprio problema de rotulação original.

**[Representação do Cromossomo — P2]**
A representação por permutação traz um desafio operacional importante: qualquer transformação aplicada a um cromossomo deve produzir outro cromossomo válido, ou seja, outra permutação sem elementos repetidos ou ausentes. Essa restrição inviabiliza o uso de operadores genéticos padrão — como cruzamento de um ponto ou mutação por substituição — e exige a adoção de operadores especializados que, por construção, preservam a estrutura permutacional dos indivíduos gerados. Os operadores implementados neste trabalho são descritos em detalhe ao longo desta seção.

**[Geração da População Inicial — P1]**
A população inicial de $p$ indivíduos é construída por amostragem aleatória do espaço de permutações: o primeiro indivíduo é a permutação identidade dos vértices, e cada um dos $p-1$ indivíduos restantes é obtido por um embaralhamento aleatório independente, garantindo diversidade desde o início da evolução. A semente do gerador de números pseudoaleatórios é obtida a partir do dispositivo de aleatoriedade do sistema operacional, assegurando que execuções independentes do algoritmo produzam populações iniciais distintas e, portanto, trajetórias evolutivas diferentes.

**[Função de Avaliação — P1]**
A aptidão de cada indivíduo é mensurada pelo span da rotulação–L(2,1) que a heurística gulosa produz quando os vértices são processados na ordem ditada pelo cromossomo. Indivíduos com menor span são considerados mais aptos e recebem maior probabilidade de participar da reprodução. O custo de avaliação de um único indivíduo é dominado pela complexidade da heurística gulosa — $\mathcal{O}(n\Delta^2)$ —, o que faz da função de avaliação o componente computacionalmente mais custoso do algoritmo, especialmente em grafos densos.

**[Operadores Genéticos — P1: Seleção]**
Para selecionar os pares de pais que gerarão a próxima geração, o AGBO utiliza o método da roleta viciada com pesos inversamente proporcionais ao fitness. Dado que o objetivo é minimizar o span, indivíduos com menores valores de fitness (spans mais baixos) recebem maior probabilidade de seleção. Isso é implementado atribuindo a cada indivíduo $i$ um peso $w_i = 1 - f_i/\sum_j f_j$, normalizando esses pesos e amostrandos pela distribuição discreta resultante. Quando todos os indivíduos possuem o mesmo fitness, a seleção degrada para uma distribuição uniforme.

**[Operadores Genéticos — P2: Cruzamento]**
Com probabilidade $p_c$, o par de pais é submetido a um operador de cruzamento para gerar dois filhos. O CX opera por identificação de ciclos de posições entre os pais: cada ciclo é atribuído alternadamente ao Pai 1 ou ao Pai 2, e os genes são copiados da permutação correspondente para o filho, preservando a posição absoluta de cada elemento. O OX, alternativamente, copia um segmento contínuo de um pai diretamente para o filho e completa as posições restantes com os elementos do outro pai na ordem circular em que aparecem. Quando o cruzamento não é realizado, os filhos são cópias integrais dos pais.

**[Operadores Genéticos — P3: Mutação por Reposicionamento]**
Após o cruzamento, cada filho é individualmente sujeito a um operador de mutação com probabilidade $p_m$. Três operadores introduzem perturbações por reposicionamento de genes: o EM troca as posições de dois genes escolhidos ao acaso, produzindo uma perturbação mínima de dois elementos; o DM remove um segmento contínuo e o reinsere em outra posição da sequência, alterando o posicionamento relativo de um bloco de genes; e o ISM extrai um único gene e o insere em uma posição diferente, funcionando como uma versão de um único elemento do DM.

**[Operadores Genéticos — P4: Mutação por Reorganização]**
Dois operadores adicionais atuam sobre a ordenação interna de segmentos: o SM seleciona um trecho da permutação e embaralha seus elementos de forma completamente aleatória, potencialmente produzindo grandes perturbações; e o SIM inverte a sequência dos elementos de um segmento delimitado por dois pontos de corte distintos. Enquanto o SM favorece diversificação ao introduzir alta aleatoriedade local, o SIM realiza uma transformação determinística e reversível, adequada para exploração estruturada do espaço de vizinhança.

**[Elitismo e Critério de Parada — P1]**
Para preservar as melhores soluções descobertas ao longo da evolução, o AGBO emprega elitismo com dois indivíduos: ao iniciar a construção de cada nova população, os dois indivíduos de menor span da geração anterior são copiados diretamente para a nova geração, antes de qualquer processo de reprodução. Esse mecanismo garante que a qualidade da melhor solução encontrada nunca regrida entre gerações, convertendo a busca em um processo de melhoria monotônica no que diz respeito ao melhor indivíduo.

**[Elitismo e Critério de Parada — P2]**
Três critérios de parada são avaliados a cada geração. O primeiro encerra o algoritmo quando o número de gerações completadas atinge o limite $G_{\max}$. O segundo monitora a estagnação da busca: um contador é incrementado a cada geração sem melhora e zerado sempre que o melhor span diminui; quando o contador atinge $S_{\max}$ gerações consecutivas sem progresso, o algoritmo para. Ambos os parâmetros $G_{\max}$ e $S_{\max}$ foram calibrados automaticamente pelo irace sobre um conjunto de instâncias representativas.

**[Elitismo e Critério de Parada — P3]**
O terceiro critério é um limite de tempo de 15 minutos por run, verificado no interior da heurística gulosa a cada vértice processado. Essa estratégia permite que avaliações de fitness em instâncias de grande porte sejam interrompidas de forma limpa no nível de granularidade de um vértice, sem risco de execuções indefinidas. Ao término da run — por qualquer dos três critérios —, o algoritmo registra o melhor span encontrado, a permutação correspondente e a rotulação obtida por ela, informações que são preservadas desde o momento de sua descoberta ao longo do processo evolutivo.

---

## VERSÃO 4 — Compacta e objetiva

### 4.2 Algoritmo Genético Baseado em Ordem (AGBO)

**[Visão Geral — P1]**
O AGBO é um algoritmo evolucionário que opera sobre permutações de vértices para resolver o Problema da Rotulação–L(2,1). Sua concepção baseia-se no fato de que a heurística gulosa produz soluções de qualidade dependente da ordem de processamento dos vértices e de que existe uma ordenação ótima que leva ao valor mínimo de $\lambda(G)$ (Lema~\ref{lema:ordenacao-otima-gulosa}). Ao evoluir uma população de permutações em vez de rotulações diretamente, o AGBO reduz o espaço de busca de forma significativa e torna o problema tratável por técnicas evolucionárias.

**[Visão Geral — P2]**
O fluxo de execução do AGBO segue o ciclo padrão dos algoritmos genéticos: avaliação da população, seleção de pais por roleta viciada, aplicação de cruzamento e mutação para geração de descendentes, preservação dos melhores indivíduos por elitismo e verificação dos critérios de parada. O processo é repetido por no máximo $G_{\max}$ gerações ou interrompido antecipadamente por estagnação ou limite de tempo, o que ocorrer primeiro.

**[Representação do Cromossomo — P1]**
O cromossomo de cada indivíduo é um vetor $\sigma \in S_n$, isto é, uma permutação dos $n$ vértices do grafo. A posição $i$ do vetor indica o $i$-ésimo vértice a ser rotulado pela heurística gulosa. O espaço de busca tem cardinalidade $n!$, o que é combinatorialmente intratável por métodos exatos, justificando o uso de uma busca heurística guiada por evolução.

**[Representação do Cromossomo — P2]**
A restrição de que os cromossomos sejam permutações válidas (sem repetições) impossibilita o uso de operadores genéticos convencionais. Por isso, foram adotados operadores especializados — CX e OX para cruzamento; EM, DM, SM, ISM e SIM para mutação — que por construção produzem apenas permutações válidas como saída, sem necessidade de etapas de reparação.

**[Geração da População Inicial — P1]**
A população inicial de $p$ indivíduos é gerada aleatoriamente: o primeiro indivíduo é a permutação identidade e os demais são embaralhamentos independentes dela, usando o gerador Mersenne Twister com semente não determinística obtida de \texttt{std::random\_device}. Cada execução produz uma população inicial distinta, garantindo independência entre runs.

**[Função de Avaliação — P1]**
A aptidão de cada cromossomo $\sigma$ é o span $\lambda_f(\sigma)$ retornado pela heurística gulosa (Algoritmo~\ref{alg:greedy}) ao processar os vértices na ordem $\sigma$. Quanto menor o span, maior a aptidão do indivíduo. O custo de cada avaliação é $\mathcal{O}(n\Delta^2)$, tornando-a o gargalo computacional do algoritmo em grafos densos.

**[Operadores Genéticos — P1: Seleção]**
A seleção por roleta viciada atribui a cada indivíduo $i$ um peso $w_i = 1 - f_i / \sum_j f_j$, favorecendo indivíduos com menor span. Os dois pais são amostrados independentemente desta distribuição. Em casos degenerados de pesos nulos (todos os indivíduos com mesmo fitness), aplica-se seleção uniforme.

**[Operadores Genéticos — P2: Cruzamento]**
Com probabilidade $p_c$, aplica-se CX ou OX ao par de pais para gerar dois filhos. O CX troca genes entre os pais com base em ciclos de posições, preservando a posição absoluta dos elementos. O OX copia um segmento de um pai e completa o filho com os elementos do outro pai em ordem circular. Com probabilidade $1 - p_c$, os filhos são cópias dos pais sem cruzamento.

**[Operadores Genéticos — P3: Mutação — EM, DM e ISM]**
Cada filho é mutado com probabilidade $p_m$. O EM troca dois genes aleatórios (perturbação mínima). O DM remove um segmento e o insere em outra posição. O ISM remove um gene e o reinsere em outro ponto da permutação. Todos produzem permutações válidas por construção.

**[Operadores Genéticos — P4: Mutação — SM e SIM]**
O SM embaralha aleatoriamente os elementos de um segmento selecionado, introduzindo perturbação de alta intensidade. O SIM inverte a ordem dos elementos de um segmento delimitado por dois pontos de corte $\ell < r$. Enquanto o SM favorece diversificação, o SIM realiza uma transformação estruturada e de intensidade moderada.

**[Elitismo e Critério de Parada — P1]**
Os dois indivíduos de menor span são copiados diretamente para a próxima geração antes da reprodução (elitismo com tamanho 2). Isso garante que a melhor solução encontrada nunca seja perdida, tornando a qualidade do melhor indivíduo monotonicamente não-crescente ao longo das gerações.

**[Elitismo e Critério de Parada — P2]**
O algoritmo encerra ao satisfazer qualquer um de dois critérios baseados no progresso: (i) atingir $G_{\max}$ gerações; ou (ii) acumular $S_{\max}$ gerações consecutivas sem melhora do melhor span (estagnação). Ambos os parâmetros foram determinados pelo irace durante a fase de calibração.

**[Elitismo e Critério de Parada — P3]**
Um terceiro critério é um limite de 15 minutos por run, verificado dentro da heurística gulosa a cada vértice processado. Caso o prazo expire antes de qualquer avaliação completa, o algoritmo reporta a penalidade $2n$ — sempre superior a qualquer span válido — assegurando que o irace possa processar o resultado mesmo para instâncias em que o greedy individualmente já excede o tempo disponível.

---

## VERSÃO 5 — Acadêmica com ênfase em decisões de projeto

### 4.2 Algoritmo Genético Baseado em Ordem (AGBO)

**[Visão Geral — P1]**
A principal decisão de projeto do AGBO é a separação entre representação e avaliação: os cromossomos codificam ordenações de vértices — objetos combinatórios simples sobre os quais é fácil definir operadores genéticos válidos —, enquanto a qualidade de cada solução é aferida por uma heurística gulosa aplicada à ordenação correspondente. Essa separação permite que o AGBO utilize um espaço de busca estruturado ($S_n$) e operadores especializados para permutações, ao mesmo tempo em que se beneficia da eficiência e da boa qualidade prática da heurística gulosa como função de avaliação.

**[Visão Geral — P2]**
A abordagem é motivada teoricamente pelo Lema~\ref{lema:ordenacao-otima-gulosa}, que assegura que a busca no espaço $S_n$ não compromete a optimalidade: existe sempre uma permutação para a qual a heurística gulosa retorna $\lambda(G)$. Na prática, contudo, o espaço $S_n$ é de tamanho $n!$ e não pode ser explorado exaustivamente. O AGBO equilibra exploração e explotação por meio de seleção proporcional ao fitness, operadores de cruzamento que preservam blocos bem adaptados, e operadores de mutação que introduzem diversidade de forma controlada.

**[Representação do Cromossomo — P1]**
Cada indivíduo da população é representado por um vetor de comprimento $n$ contendo uma permutação dos identificadores dos vértices do grafo $G = (V, E)$, com $|V| = n$. A posição $k$ do vetor indica qual vértice deve ser o $k$-ésimo a receber um rótulo durante a execução da heurística gulosa. Essa representação é natural para o problema e alinha-se diretamente com a estrutura da heurística de avaliação, sem necessidade de qualquer etapa de decodificação intermediária.

**[Representação do Cromossomo — P2]**
A escolha de uma representação permutacional impõe, como contrapartida, a necessidade de operadores genéticos que garantam a viabilidade das soluções geradas. Operadores padrão — como cruzamento de um ponto ou dois pontos — produzem, quando aplicados a permutações, filhos com elementos duplicados ou ausentes, tornando-os inviáveis. Para evitar esse problema, foram implementados operadores projetados especificamente para permutações que, por construção algébrica, sempre produzem saídas válidas sem necessidade de reparação posterior.

**[Geração da População Inicial — P1]**
A população inicial é construída por amostragem aleatória uniforme de $S_n$: o primeiro indivíduo é a permutação identidade e cada um dos $p-1$ indivíduos restantes é gerado por um embaralhamento de Fisher-Yates independente, usando o gerador Mersenne Twister semeado por \texttt{std::random\_device}. A escolha de semeadura não determinística assegura que múltiplas execuções do algoritmo produzam populações iniciais distintas, contribuindo para a independência estatística das runs e para a validade das análises comparativas realizadas posteriormente.

**[Função de Avaliação — P1]**
A função de avaliação do AGBO é a composição da heurística gulosa com a permutação cromossômica: $\text{fitness}(\sigma) = \text{Greedy}(G, \sigma).\lambda$, onde $\text{Greedy}(G, \sigma).\lambda$ é o span retornado pela heurística ao processar os vértices de $G$ na ordem definida por $\sigma$. A função é sempre bem definida — a heurística sempre termina com uma rotulação válida —, mas seu custo computacional de $\mathcal{O}(n\Delta^2)$ por chamada torna a avaliação o gargalo da execução, especialmente em grafos de alta densidade onde $\Delta \approx n$.

**[Operadores Genéticos — P1: Seleção]**
A seleção por roleta viciada foi adotada por sua capacidade de manter pressão seletiva proporcional à diferença de qualidade entre os indivíduos. Para cada geração, os pesos são recalculados dinamicamente com base nos fitness atuais da população, evitando que a pressão seletiva colapse em fases tardias da evolução nas quais os fitnesses tendem a convergir. O mecanismo de fallback para seleção uniforme garante robustez em populações completamente convergidas, sem interromper prematuramente o processo evolutivo.

**[Operadores Genéticos — P2: Cruzamento]**
O CX e o OX foram implementados por serem os operadores de cruzamento para permutações mais bem estudados na literatura de problemas combinatórios e por apresentarem propriedades complementares: o CX tende a preservar a posição absoluta dos genes (relevante quando a posição de um vértice na ordenação é informativa), enquanto o OX preserva a ordem relativa dos genes (relevante quando o que importa é a sequência relativa dos vértices). A taxa de cruzamento $p_c$, calibrada pelo irace, controla o equilíbrio entre exploração e preservação das soluções parentais.

**[Operadores Genéticos — P3: Mutação por Reposicionamento]**
Os operadores de mutação por reposicionamento — EM, DM e ISM — atuam deslocando elementos dentro da permutação sem alterar o conjunto de elementos presentes. O EM, por ser o operador de menor perturbação (apenas dois genes são afetados), é adequado para ajustes finos em regiões promissoras do espaço de busca. O DM e o ISM introduzem perturbações de amplitude variável, controlada pelo tamanho do segmento ou pela distância de reinserção, permitindo saltos de diferentes magnitudes no espaço de permutações.

**[Operadores Genéticos — P4: Mutação por Reorganização Interna]**
O SM e o SIM diferem dos operadores de reposicionamento por não alterarem a posição do bloco na permutação, mas sim a ordem interna de seus elementos. O SM, ao embaralhar aleatoriamente o segmento, é o operador de maior potencial de diversificação dentre os implementados, sendo particularmente útil quando a população apresenta sinais de convergência prematura. O SIM, com sua inversão determinística, é geometricamente equivalente a um movimento de 2-opt no espaço de permutações, operação que demonstrou eficiência em problemas de otimização de sequenciamento.

**[Elitismo e Critério de Parada — P1]**
O elitismo com dois indivíduos foi adotado como mecanismo de preservação das melhores soluções ao longo da evolução. Esse tamanho de elite representa um compromisso: é suficientemente pequeno para não dominar a pressão seletiva da roleta viciada (que poderia ocorrer com elites maiores), mas suficientemente grande para garantir que os dois melhores padrões genéticos da geração sejam transmitidos à próxima. A presença dos dois melhores — e não apenas do melhor — também mantém alguma diversidade genética entre os indivíduos preservados.

**[Elitismo e Critério de Parada — P2]**
Os critérios de parada por número máximo de gerações ($G_{\max}$) e por estagnação ($S_{\max}$) foram calibrados conjuntamente pelo irace, pois suas interações determinam o comportamento temporal do algoritmo. Um valor alto de $G_{\max}$ com $S_{\max}$ baixo favorece terminação antecipada e eficiência; um $S_{\max}$ alto com $G_{\max}$ moderado permite exploração mais longa em busca de melhorias tardias. A configuração vencedora identificada pelo irace — $G_{\max} = 53$ e $S_{\max} = 1031$ — sugere que o algoritmo tende a estagnar rapidamente, mas que a estagnação nem sempre é definitiva, justificando um limiar alto para o contador de parada.

**[Elitismo e Critério de Parada — P3]**
O limite de 15 minutos por run é implementado por um \textit{deadline} absoluto passado como parâmetro à heurística gulosa, que o verifica no início do processamento de cada vértice. Essa estratégia é preferível à verificação apenas entre avaliações completas porque, em grafos de alta densidade, uma única avaliação de fitness pode exceder o limite de tempo — situação que ocorre para instâncias como C1000-9, na qual o custo de uma chamada greedy ultrapassa os 15 minutos disponíveis. Nesses casos, o algoritmo retorna uma penalidade de $2n$ — valor acima de qualquer span válido —, garantindo que o processo de calibração e execução prossiga sem interrupções.
