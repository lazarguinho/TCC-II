Nesse modelo generalizado, assume-se um grafo $G = (V, E)$, e para cada vértice $v \in V$ e cada inteiro $i \in \{0, 1, \ldots, k\}$, define-se uma variável binária $x_{v,i}$, a qual assume valor 1 se o vértice $v$ for rotulado com o valor $i$, e 0 caso contrário. O modelo pode ser formulado como:

$$
\begin{aligned}
\min \quad & z \\
\text{sujeito a:} \\
& \sum_{i=0}^{k} x_{v,i} = 1,
&& \forall v \in V, \\
& x_{v,i} + x_{u,j} \leq 1,
&& \forall v,u \in V,\; 0 \leq i,j \leq k, \\
&&& \text{se } d(v,u)=q,\; |i-j| < k_q,\; 1 \leq q \leq n, \\
& i\,x_{v,i} \leq z,
&& \forall v \in V,\; 0 \leq i \leq k, \\
& x_{v,i} \in \{0,1\},
&& \forall v \in V,\; 0 \leq i \leq k.
\end{aligned}
$$

O objetivo do modelo é minimizar o maior rótulo utilizado, representado pela variável $z$. A primeira restrição garante que cada vértice receba exatamente um rótulo. A segunda assegura a viabilidade da rotulação, impedindo que dois vértices a uma distância $q$ recebam rótulos cuja diferença seja inferior a $k_q$. A terceira restrição garante que o maior rótulo atribuído a qualquer vértice não ultrapasse o valor da variável $z$, que está sendo minimizada. Por fim, a última restrição define que as variáveis de decisão são binárias.

Esse modelo abrange como caso particular o problema de rotulação-$L(2,1)$, que corresponde à escolha $k_1 = 2$ e $k_2 = 1$. As restrições apresentadas podem ser linearizadas com variáveis auxiliares, viabilizando o uso de solvers como CPLEX, Gurobi ou GLPK.