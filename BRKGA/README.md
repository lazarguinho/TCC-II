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
3. inicializa o BRKGA;
4. evolui a população ao longo das gerações;
5. interrompe quando atinge um critério de parada;
6. salva o melhor resultado encontrado.

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

---

## Organização dos arquivos

### `src/`

Contém o código-fonte principal.

- `main.cpp`  
  Controla leitura da instância, parsing dos argumentos, execução do BRKGA e escrita dos resultados.

- `decoder.cpp` / `decoder.h`  
  Implementam o decodificador que transforma cromossomos em soluções viáveis do problema.

- `greedy.cpp` / `greedy.hpp`  
  Implementam heurísticas auxiliares.

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

## Estratégia de execução

Para cada run:

1. o grafo é lido do arquivo `.mtx`;
2. o número de genes do cromossomo é definido como o número de vértices do grafo;
3. um `Decoder` é instanciado;
4. o gerador aleatório usa a semente igual ao índice da execução:
   - `rngSeed = r`
5. o BRKGA é executado;
6. a melhor solução encontrada é decodificada;
7. os resultados são salvos em arquivo.

---

## Observações de implementação

- O tipo do grafo usado em `main.cpp` é:

```cpp
using Graph = std::vector<std::vector<int>>;
```

ou seja, uma lista de adjacência.

- A leitura do `.mtx` ignora o valor da terceira coluna, usando apenas os pares de vértices.

- O tamanho do cromossomo (`n`) é definido automaticamente como:

```cpp
n = número de vértices do grafo
```

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
- documentar formalmente o `Decoder` e a heurística gulosa.
