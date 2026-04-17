# BRKGA para L(2,1) Labeling em Grafos

Este projeto implementa um **BRKGA (Biased Random-Key Genetic Algorithm)** para resolver o problema de **L(2,1) labeling** em grafos.

A aplicação lê uma instância de grafo em formato **Matrix Market (`.mtx`)**, executa o algoritmo evolutivo e salva os resultados em arquivo, incluindo:

- melhor valor encontrado para o span (`lambda`);
- ordem usada pelo decodificador;
- rotulação final dos vértices;
- histórico de convergência.

---

## Objetivo

O programa resolve instâncias do problema **L(2,1) labeling** usando BRKGA.

De forma geral, o algoritmo:

1. lê um grafo a partir de um arquivo `.mtx`;
2. constrói a representação em lista de adjacência;
3. transforma cada cromossomo em uma ordem de vértices;
4. aplica uma heurística gulosa para gerar a rotulação;
5. valida a solução produzida;
6. usa o valor da solução como fitness no BRKGA;
7. salva o melhor resultado encontrado.

---

## Formato de entrada

O programa espera um arquivo no formato **Matrix Market (`.mtx`)**, no estilo **coordinate**.

A leitura feita em `main.cpp` assume:

- primeira linha com o cabeçalho `%%MatrixMarket`;
- linhas iniciadas por `%` são comentários;
- depois disso, uma linha com:
  - número de linhas,
  - número de colunas,
  - número de entradas não nulas (`nnz`);
- cada aresta é lida com **3 colunas**:
  - vértice `u`
  - vértice `v`
  - valor `val` (lido, mas ignorado pelo programa)

Exemplo simplificado:

```text
%%MatrixMarket matrix coordinate integer symmetric
% comentário
8 8 12
1 2 1
1 3 1
2 4 1
...
```

### Observações importantes

- Os índices dos vértices no arquivo devem começar em **1**.
- O programa converte internamente para índices começando em **0**.
- O grafo é tratado como **simétrico**:
  - ao ler `(u, v)`, o programa adiciona `v` em `u` e `u` em `v`;
  - laços `(u = v)` não são duplicados.
- O valor da terceira coluna é consumido na leitura, mas não é utilizado na modelagem do grafo.

---

## Organização dos arquivos

### `src/`

Contém o código-fonte principal.

- `main.cpp`  
  Controla leitura da instância, parsing dos argumentos, execução do BRKGA e escrita dos resultados.

- `decoder.cpp` / `decoder.h`  
  Implementam o decodificador que transforma cromossomos em soluções viáveis do problema.

- `greedy.cpp` / `greedy.hpp`  
  Implementam a heurística gulosa usada para construir a rotulação L(2,1) a partir da ordem dos vértices.

- `Graph.cpp` / `Graph.h`  
  Estruturas e rotinas relacionadas ao grafo.

- `brkgaAPI/`  
  Implementação da biblioteca usada pelo BRKGA:
  - `BRKGA.h`
  - `Population.cpp`
  - `Population.h`
  - `MTRand.h`

### `data/`

Contém as instâncias de entrada.

### `results/`

Armazena os arquivos de saída gerados automaticamente.

### `build/`

Arquivos objeto gerados na compilação.

### `bin/`

Executável gerado pelo projeto.

### `tuning/`

Arquivos relacionados ao ajuste automático de parâmetros do BRKGA.

---

## Requisitos

Para compilar o projeto, você precisa de:

- compilador C++ com suporte a **C++17** ou superior;
- `make`.

No macOS, isso normalmente funciona com:

- `clang++`
- Xcode Command Line Tools instalados

---

## Compilação

Para compilar o projeto:

```bash
make
```

Após a compilação, o executável deve estar em:

```bash
./bin/main
```

---

## Uso

```bash
./bin/main <caminho.mtx> [opções]
```

### Exemplo básico

```bash
./bin/main data/teste/johnson8-2-4_clean.mtx
```

### Exemplo com múltiplas execuções

```bash
./bin/main data/teste/johnson8-2-4_clean.mtx --runs 10
```

### Ajuda

```bash
./bin/main --help
```

---

## Parâmetros disponíveis

| Opção | Descrição | Valor padrão |
|---|---|---:|
| `--runs <N>` | número de execuções independentes | `1` |
| `--p <N>` | tamanho da população | `100` |
| `--pe <x>` | fração de indivíduos elite | `0.2` |
| `--pm <x>` | fração de mutantes | `0.1` |
| `--rhoe <x>` | probabilidade de herança do pai elite | `0.5` |
| `--K <N>` | número de populações paralelas | `6` |
| `--MAXT <N>` | número máximo de threads | `6` |
| `--X_INTVL <N>` | intervalo de troca entre populações | `100` |
| `--X_NUMBER <N>` | número de indivíduos elite trocados | `2` |
| `--MAX_GENS <N>` | número máximo de gerações | `1000` |
| `--MAX_STAGT <x>` | máximo de gerações sem melhora | `400` |
| `-h`, `--help` | mostra a ajuda | — |

---

## Como o decoder funciona

O `Decoder` recebe o grafo por referência e implementa o método:

```cpp
double decode(const std::vector<double>& chrom) const;
```

Esse método é o elo entre o BRKGA e a heurística de construção da solução.

### Etapas do `decode`

1. **Validação do tamanho do cromossomo**  
   O número de chaves aleatórias deve ser igual ao número de vértices do grafo.  
   Caso contrário, o método lança exceção.

2. **Construção de `best_order_`**  
   O vetor `[0, 1, ..., n-1]` é ordenado por `stable_sort` de acordo com os valores do cromossomo.  
   Isso transforma o cromossomo em uma **ordem de processamento dos vértices**.

3. **Execução da heurística gulosa**  
   O decoder chama:

   ```cpp
   GreedyResult gr = greedy_labeling(g, best_order_);
   ```

   A heurística constrói a rotulação L(2,1) seguindo a ordem induzida pelo cromossomo.

4. **Armazenamento da solução**  
   O vetor de rótulos produzido pela heurística é salvo em `labeling_`.

5. **Validação da solução**  
   O decoder verifica se a rotulação respeita as restrições do problema:
   - para vértices a distância 1: `|f(u) - f(v)| >= 2`
   - para vértices a distância 2: `|f(u) - f(w)| >= 1`

6. **Retorno do fitness**  
   - se a solução for válida, o decoder retorna `gr.k`, isto é, o span da solução;
   - se a solução for inválida, retorna `1e18`, penalizando fortemente o indivíduo.

---

## Validação L(2,1)

Em `decoder.cpp`, a função auxiliar `validate_L21(...)` verifica se a solução gerada pela heurística respeita as restrições do problema.

### Restrições checadas

#### Distância 1
Para cada aresta `(u, v)`:

```text
|f(u) - f(v)| >= 2
```

#### Distância 2
Para cada par de vértices `(u, w)` com distância 2:

```text
|f(u) - f(w)| >= 1
```

Na prática, a checagem de distância 2 é feita percorrendo, para cada vértice `u`:

- seus vizinhos `v`;
- e depois os vizinhos `w` de cada `v`.

Se alguma violação for encontrada, a função devolve `false` e informa os vértices e a distância onde ocorreu o problema.

---

## Estado interno do decoder

O `Decoder` mantém três vetores mutáveis para permitir atualização mesmo dentro de um método `const`:

- `best_order_`: ordem dos vértices induzida pelo cromossomo;
- `labeling_`: rotulação construída pela heurística gulosa;
- `convergence_`: histórico dos melhores valores por geração.

Esses vetores são expostos pelos getters:

```cpp
best_order()
labeling()
convergence()
```

O histórico de convergência é alimentado no `main.cpp` por:

```cpp
decoder.clear_convergence();
decoder.push_convergence(best_lambda_so_far);
```

---

## Critérios de parada

Cada execução do BRKGA termina quando ocorre uma das condições abaixo:

1. atinge o número máximo de gerações:
   - `generation >= MAX_GENS`
2. atinge o limite de estagnação:
   - `stagnant_count >= MAX_STAGT`
3. atinge o limite de tempo:
   - **15 minutos por execução**

Durante a execução, o programa também realiza troca de elite entre populações a cada:

```text
X_INTVL gerações
```

trocando:

```text
X_NUMBER melhores indivíduos
```

---

## Saída do programa

### Saída no terminal

Ao final, o programa imprime no terminal apenas:

- o melhor valor global encontrado entre todas as execuções.

### Arquivo de saída

Para cada instância executada, o programa cria automaticamente um arquivo em `results/` com o nome:

```text
results/<nome_do_arquivo>.mtx_result.txt
```

Por exemplo, para a entrada:

```text
data/teste/johnson8-2-4_clean.mtx
```

o arquivo gerado será:

```text
results/johnson8-2-4_clean.mtx_result.txt
```

---

## Conteúdo do arquivo de resultado

O arquivo de saída contém, para cada execução:

- número da execução;
- motivo da parada;
- tempo de execução em milissegundos;
- melhor `span (lambda)` encontrado;
- `best_order`;
- `labeling`;
- vetor de `convergência`.

Exemplo de estrutura:

```text
===== RESULTADO BRKGA L(2,1) =====
Arquivo: data/teste/johnson8-2-4_clean.mtx

----- Execução #1 -----
[INFO] Parou por MAX_GENS.
Tempo de execução: 1234ms
Span (lambda): 17
best_order = [ ... ]
labeling = [ ... ]
convergência = [ ... ]

===== FIM =====
```

---

## Fluxo geral da execução

Para cada run:

1. o grafo é lido do arquivo `.mtx`;
2. o número de genes do cromossomo é definido como o número de vértices do grafo;
3. um `Decoder` é instanciado;
4. o gerador aleatório usa a semente igual ao índice da execução:
   - `rngSeed = r`
5. o BRKGA evolui a população;
6. a cada geração, o melhor fitness é atualizado;
7. a convergência é armazenada;
8. ao final, o melhor cromossomo é decodificado novamente;
9. os resultados são escritos em arquivo.

---

## Observações de implementação

- O tipo do grafo usado no projeto é:

```cpp
using Graph = std::vector<std::vector<int>>;
```

ou seja, uma lista de adjacência.

- O `decode(...)` é `const` porque a API do BRKGA exige essa assinatura.

- Para contornar isso, o decoder usa atributos `mutable` para salvar:
  - a ordem da melhor solução;
  - a rotulação correspondente;
  - a convergência.

- A leitura do `.mtx` ignora o valor da terceira coluna, usando apenas os pares de vértices.

- O tamanho do cromossomo (`n`) é definido automaticamente como o número de vértices do grafo.

- O melhor fitness global entre todas as execuções é armazenado em `overall_best`.

---

## Exemplo de execução

```bash
./bin/main data/teste/johnson8-2-4_clean.mtx --runs 5 --p 200 --pe 0.2 --pm 0.1 --rhoe 0.7 --K 4 --MAXT 4 --X_INTVL 50 --X_NUMBER 2 --MAX_GENS 2000 --MAX_STAGT 300
```

---

## Tuning de parâmetros

A pasta `tuning/` reúne arquivos para experimentos de calibração dos parâmetros do BRKGA, como:

- tamanho da população;
- percentual de elite;
- percentual de mutantes;
- probabilidade de recombinação;
- número de populações;
- limites de gerações.

Isso permite testar combinações de parâmetros em várias instâncias e identificar configurações mais eficientes.

---

## Possíveis melhorias futuras

- adicionar logs mais detalhados no terminal;
- salvar estatísticas agregadas de múltiplas execuções;
- calcular média, desvio padrão e melhor resultado final;
- suportar mais formatos de entrada;
- validar automaticamente se o `.mtx` é realmente simétrico;
- documentar formalmente `GreedyResult` e a função `greedy_labeling(...)`.
