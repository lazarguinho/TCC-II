# Grafos utilizados no artigo

Neste artigo, os experimentos foram realizados **exclusivamente sobre
cubos de Fibonacci** (Fibonacci Cubes), representados por $\Gamma_n$. Os
autores testaram instâncias de diferentes tamanhos, variando a dimensão
$n$.

            Grafo   Nº de vértices
  --------------- ----------------
       $\Gamma_2$                3
       $\Gamma_3$                5
       $\Gamma_4$                8
       $\Gamma_5$               13
       $\Gamma_6$               21
       $\Gamma_7$               34
       $\Gamma_8$               55
       $\Gamma_9$               89
    $\Gamma_{10}$              144
    $\Gamma_{11}$              233
    $\Gamma_{12}$              377
    $\Gamma_{13}$              610
    $\Gamma_{14}$              987
    $\Gamma_{15}$             1597
    $\Gamma_{16}$             2584

Os autores afirmam que:

> "The algorithms have been tested on the Fibonacci cubes $\Gamma_n$
> with up to 2584 vertices ($n \le 16$)." (Artigo, p. 5)

## Como esses grafos são definidos?

O cubo de Fibonacci $\Gamma_n$ é um subgrafo do hipercubo $Q_n$:

-   os vértices são todas as cadeias binárias de comprimento $n$ **sem
    dois 1's consecutivos**;
-   dois vértices são adjacentes quando diferem em exatamente um bit
    (distância de Hamming igual a 1).

Por exemplo:

-   $\Gamma_3$: 5 vértices
-   $\Gamma_4$: 8 vértices
-   $\Gamma_5$: 13 vértices

O número de vértices segue exatamente a sequência de Fibonacci:

$$
5,\;8,\;13,\;21,\;34,\;55,\;89,\ldots
$$

## Observação

Embora o artigo cite trabalhos anteriores sobre **hipercubos** ($Q_n$),
eles aparecem apenas como comparação teórica e motivação do problema. Os
experimentos deste trabalho **não utilizam hipercubos**, apenas cubos de
Fibonacci.
