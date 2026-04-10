# PLI

Projeto para modelagem e resolução do problema de **rotulação-L(2,1)** em grafos por meio de **Programação Linear Inteira (PLI)**, com uso de **Python**, **NetworkX**, **SciPy**, **DOcplex** e **OR-Tools**.

Além da modelagem matemática, o projeto inclui uma rotina experimental para:

- ler instâncias de grafos no formato `.mtx`;
- construir uma solução inicial gulosa;
- resolver o modelo inteiro com solver MIP;
- salvar a rotulação encontrada em `.csv`;
- salvar métricas da instância e da execução em `.csv`.

---

## Formulação implementada

O projeto contém duas implementações principais da modelagem:

- `main.py`: versão usando **OR-Tools**;
- `main copy.py`: versão experimental usando **DOcplex/CPLEX**, leitura de arquivos `.mtx`, geração de solução inicial gulosa e processamento em lote.

### Variáveis de decisão

A formulação usa:

- \(x_i\): rótulo inteiro não negativo associado ao vértice \(i\);
- \(z\): maior rótulo utilizado na solução;
- \(b_{ij}\): variável binária usada para linearizar as restrições entre vértices a distância 1;
- \(d_{ij}\): variável binária usada para linearizar as restrições entre vértices a distância 2.

### Restrições

Para todo vértice \(i\):

- \(x_i \le z\)

Para cada par de vértices adjacentes \((i,j)\):

- \(|x_i - x_j| \ge 2\)

Para cada par de vértices a distância 2:

- \(|x_i - x_j| \ge 1\)

Como o modelo é linear inteiro, as desigualdades com valor absoluto são linearizadas com constantes do tipo **big-M**.

### Função objetivo

Minimizar:

- \(z\)

ou seja, minimizar o maior rótulo usado.

---

## Descrição da estrutura do projeto

### Instâncias de entrada

* `data/`
Contém instâncias de grafos no formato `.mtx`, para grafos aleatórios do tipo Erdős-Rényi.
* `NR_DIMACS_cleaned/`
Contém instâncias limpas da coleção `NR/DIMACS` no formato `.mtx` que não foram processadas.
* `Harwell-Boeing_cleaned/`
Contém instâncias limpas do conjunto `Harwell-Boeing` no formato `.mtx` que não foram processadas.
* `teste/`
Contém instâncias do conjunto de teste no formato `.mtx` que não foram processadas.

> **Nota:** Isso se deu porque depois de processar as instâncias aleatórias, passei a remover as instâncias processadas com sucesso de seus diretórios.
> Pretendo mover de volta as instâncias processadas para os diretórios correspondentes ou centralizar as instâncias em um diretório.

### Rotulações-L(2,1)

* `labelings_data/`
Armazena as rotulações geradas para as instâncias presentes em `data/`.

* `labelings_NR_DIMACS_cleaned/`
Armazena as rotulações geradas para as instâncias da coleção `NR/DIMACS` limpas.

* `labelings_Harwell-Boeing_cleaned/`
Armazena as rotulações geradas para as instâncias do conjunto Harwell-Boeing limpas.

* `labelings_teste/`
Armazena as rotulações geradas para as instâncias do conjunto de teste.

### Resultados

* `results_data/`
Contém os resultados das execuções para as instâncias em `data/`.

* `results_NR_DIMACS_cleaned/`
Contém os resultados das execuções para as instâncias da coleção `NR_DIMACS_cleaned/`.

* `results_Harwell-Boeing_cleaned/`
Contém os resultados das execuções para as instâncias do conjunto `Harwell-Boeing_cleaned/`.

* `results_teste/`
Contém os resultados das execuções para as instâncias do conjunto de teste.

### Execuções anteriores

* `Primeira execução sem solução inicial/`
Guarda resultados de uma execução anterior do projeto, realizada sem fornecer uma solução inicial para o solver. Esse diretório contém:
  * `labelings_data/`
  * `results_data/`

> **Nota:** Quando comecei a executar o PLI notei que o solver não conseguia gerar a primeira solução antes do prazo de 15min, então decidi fornecer uma solução inicial para ele atráves de um algoritmo guloso.
> Muitas resultados retornaram essa solução inicial, a vantagem desta estratégia é que pelo menos conseguimos obter um resultado para comparar com os algoritmos genéticos.

### Arquivos principais

* `main.py`
* `main copy.py`
