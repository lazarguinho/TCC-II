# PLI

Projeto para modelagem e resolução do problema de **rotulação-L(2,1)** em grafos por meio de **Programação Linear Inteira (PLI)**, com uso de **Python**, **NetworkX**, **SciPy**, **DOcplex** e **OR-Tools**.

Além da modelagem matemática, o projeto inclui uma rotina experimental para:

- ler instâncias de grafos no formato `.mtx`;
- construir uma solução inicial gulosa;
- resolver o modelo inteiro com solver MIP;
- salvar a rotulação encontrada em `.csv`;
- salvar métricas da instância e da execução em `.csv`.

<details>
<summary>Sumário</summary>

## Sumário

- [Formulação implementada](#formulação-implementada)
  - [Variáveis de decisão](#variáveis-de-decisão)
  - [Restrições](#restrições)
  - [Função objetivo](#função-objetivo)
- [Estratégia computacional](#estratégia-computacional)
- [Descrição da estrutura do projeto](#descrição-da-estrutura-do-projeto)
  - [Instâncias de entrada](#instâncias-de-entrada)
  - [Pastas de saída](#pastas-de-saída)
    - [Rotulações-L(2,1)](#rotulações-l21)
    - [Resultados](#resultados)
  - [Execuções anteriores](#execuções-anteriores)
  - [Arquivos principais](#arquivos-principais)
- [Dependências](#dependências)
- [Como executar](#como-executar)
  - [Versão simples com OR-Tools](#versão-simples-com-or-tools)
  - [Versão experimental em lote com DOcplex/CPLEX](#versão-experimental-em-lote-com-docplexcplex)
- [Parâmetros relevantes](#parâmetros-relevantes)
- [Status das soluções](#status-das-soluções)
- [Exemplo de saída](#exemplo-de-saída)
  - [Arquivo de rotulação](#arquivo-de-rotulação)
  - [Arquivo de resultado](#arquivo-de-resultado)
- [Melhorias futuras](#melhorias-futuras)

</details>

## Formulação implementada

O projeto contém duas implementações principais da modelagem:

- `main.py`: versão usando **OR-Tools**;
- `main copy.py`: versão experimental usando **DOcplex/CPLEX**, com leitura de arquivos `.mtx`, geração de solução inicial gulosa e processamento em lote.

### Variáveis de decisão

A formulação usa:

- $x_i$: rótulo inteiro não negativo associado ao vértice $i$;
- $z$: maior rótulo utilizado na solução;
- $b_{ij}$: variável binária usada para linearizar as restrições entre vértices a distância 1;
- $d_{ij}$: variável binária usada para linearizar as restrições entre vértices a distância 2.

### Restrições

Para todo vértice $i$:

- $x_i \le z$

Para cada par de vértices adjacentes $(i,j)$:

- $|x_i - x_j| \ge 2$

Para cada par de vértices a distância 2:

- $|x_i - x_j| \ge 1$

Como o modelo é linear inteiro, as desigualdades com valor absoluto são linearizadas com constantes do tipo **big-M**.

### Função objetivo

Minimizar:

- $z$

ou seja, minimizar o maior rótulo usado.

## Estratégia computacional

A rotina implementada em `main copy.py` segue as etapas abaixo para cada instância:

1. lê o grafo a partir de um arquivo `.mtx`;
2. remove laços;
3. calcula informações estruturais da instância;
4. gera uma **solução inicial gulosa**;
5. valida a solução gulosa;
6. calcula os conjuntos de vizinhos a distância 1 e 2;
7. constrói o modelo de PLI no **CPLEX/DOcplex**;
8. adiciona a solução gulosa como **MIP start**, quando ela é válida;
9. resolve o modelo com limite de tempo;
10. salva:
    - a rotulação em `labelings_*`;
    - o resumo da execução em `results_*`;
11. remove da pasta de entrada a instância processada com sucesso.

## Descrição da estrutura do projeto

### Instâncias de entrada

- `data/`  
  Contém instâncias de grafos aleatórios do tipo Erdős-Rényi no formato `.mtx`.

- `NR_DIMACS_cleaned/`  
  Contém instâncias limpas da coleção `NR/DIMACS` no formato `.mtx` que não foram processadas.

- `Harwell-Boeing_cleaned/`  
  Contém instâncias limpas do conjunto `Harwell-Boeing` no formato `.mtx` que não foram processadas.

- `teste/`  
  Contém instâncias do conjunto de teste no formato `.mtx` que não foram processadas.

> **Nota:** no fluxo atual de experimentos, após o processamento bem-sucedido de uma instância, o arquivo `.mtx` correspondente é removido da pasta de entrada. Essa estratégia foi adotada como forma de controle das instâncias já processadas e passou a ser usada em todos os conjuntos, com exceção do conjunto de grafos aleatórios, que foi processado antes da implementação dessa remoção. Em versões futuras, a organização poderá ser centralizada em uma única pasta de instâncias.

### Pastas de saída

#### Rotulações-L(2,1)

- `labelings_data/`  
  Armazena as rotulações geradas para as instâncias de `data/`.

- `labelings_NR_DIMACS_cleaned/`  
  Armazena as rotulações geradas para as instâncias de `NR_DIMACS_cleaned/`.

- `labelings_Harwell-Boeing_cleaned/`  
  Armazena as rotulações geradas para as instâncias de `Harwell-Boeing_cleaned/`.

- `labelings_teste/`  
  Armazena as rotulações geradas para as instâncias do conjunto de teste.

Cada arquivo de rotulação é salvo no formato `.csv` com duas colunas:

```text
vertex,label
```

#### Resultados

- `results_data/`  
  Contém os resultados das execuções para as instâncias em `data/`.

- `results_NR_DIMACS_cleaned/`  
  Contém os resultados das execuções para as instâncias em `NR_DIMACS_cleaned/`.

- `results_Harwell-Boeing_cleaned/`  
  Contém os resultados das execuções para as instâncias em `Harwell-Boeing_cleaned/`.

- `results_teste/`  
  Contém os resultados das execuções para as instâncias do conjunto de teste.

Cada arquivo de resultado é salvo em `.csv` com o cabeçalho:

```text
graph,#vertices,#edges,density,max_degree,min_degree,time(ms),lambda,status
```

Significado das colunas:

- **`graph`**: nome da instância;
- **`#vertices`**: número de vértices do grafo;
- **`#edges`**: número de arestas do grafo;
- **`density`**: densidade do grafo;
- **`max_degree`**: maior grau do grafo;
- **`min_degree`**: menor grau do grafo;
- **`time(ms)`**: tempo de resolução em milissegundos;
- **`lambda`**: valor do *span* encontrado;
- **`status`**: status da solução retornada pelo solver.

### Execuções anteriores

- `Primeira execução sem solução inicial/`  
  Guarda resultados de uma execução anterior do projeto, realizada sem fornecer solução inicial ao solver. Esse diretório contém:
  - `labelings_data/`
  - `results_data/`

> **Nota:** nas primeiras execuções, observou-se que o solver frequentemente não conseguia encontrar uma solução viável dentro do limite de 15 minutos. Por isso, foi incorporada uma solução inicial gulosa. Mesmo quando o solver não consegue provar a otimalidade, essa estratégia permite obter ao menos uma boa solução viável para comparação com abordagens genéticas.

### Arquivos principais

- `main.py`  
  Implementa o modelo de rotulação-L(2,1) com **OR-Tools**.

- `main copy.py`  
  Implementa a rotina experimental com **DOcplex/CPLEX**, incluindo:
  - leitura de arquivos `.mtx`;
  - geração de solução inicial gulosa;
  - validação da solução inicial;
  - cálculo de vizinhos a distância 1 e 2;
  - uso de **MIP start**;
  - salvamento de resultados e rotulações.

## Dependências

- `networkx`
- `scipy`
- `docplex`
- `ortools`
- `tqdm`

Instalação via `pip`:

```bash
pip install networkx scipy docplex ortools tqdm
```

> **Importante:** para executar a versão com **DOcplex**, é necessário ter acesso a um solver compatível, como o **IBM CPLEX**.

## Como executar

### Versão simples com OR-Tools

Para testar a formulação em um grafo de exemplo:

```bash
python main.py
```

Essa versão cria um ciclo $C_5$ e resolve o problema de rotulação-L(2,1) para esse grafo.

### Versão experimental em lote com DOcplex/CPLEX

A versão `main copy.py` processa todas as instâncias `.mtx` da pasta configurada na variável:

```python
input_directory_name = "NR_DIMACS_cleaned"
```

Para executar:

```bash
python "main copy.py"
```

Durante a execução, o script:

- cria automaticamente as pastas de saída, se necessário;
- processa cada instância da pasta de entrada;
- salva um `.csv` de resultado;
- salva um `.csv` de rotulação;
- remove a instância processada da pasta de entrada.

## Parâmetros relevantes

Na rotina experimental, o principal parâmetro configurável é:

```python
TIME_LIMIT_MINUTES = 15
```

Esse valor define o limite de tempo do solver para cada instância.

Além disso, o modelo usa constantes `big-M` baseadas no grau máximo do grafo:

- $M = \Delta^2 + \Delta + 2$
- $L = \Delta^2 + \Delta + 1$

onde $\Delta$ representa o maior grau do grafo.

## Status das soluções

Na rotina experimental, o campo `status` pode indicar, por exemplo:

- `OPTIMAL`: solução ótima encontrada;
- `BEST FOUND`: melhor solução viável encontrada dentro do limite de tempo.

## Exemplo de saída

### Arquivo de rotulação

```csv
vertex,label
0,0
1,2
2,4
3,1
4,3
```

### Arquivo de resultado

```csv
graph,#vertices,#edges,density,max_degree,min_degree,time(ms),lambda,status
C5,5,5,0.50000,2,2,123.45,4,OPTIMAL
```

## Melhorias futuras

- [ ] remover `main.py` e consolidar o conteúdo em uma única versão organizada baseada em `main copy.py`;
- [ ] criar uma interface por argumentos de linha de comando;
- [ ] tornar opcional a remoção automática dos arquivos `.mtx` processados;
- [ ] incluir logs consolidados de execução;
- [ ] centralizar os diretórios de entrada;
- [ ] centralizar os resultados em um único `.csv`.
