import os
import networkx as nx
from scipy.io import mmread, mmwrite 
from scipy.sparse import issparse, csr_matrix
from datetime import datetime

INPUT_DIR = "data/NR_DIMACS"
OUTPUT_DIR = "data/NR_DIMACS_cleaned"
LOG_FILE = os.path.join(OUTPUT_DIR, "process.log")

def ensure_dir(path):
    """Garante que o diretório de saída exista."""
    if not os.path.exists(path):
        os.makedirs(path)

def log(msg: str):
    """Salva mensagens no arquivo de log."""
    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    with open(LOG_FILE, "a") as f:
        f.write(f"[{timestamp}] {msg}\n")

def clean_graphs():
    ensure_dir(OUTPUT_DIR)

    # inicia o arquivo zerando conteúdo
    open(LOG_FILE, "w").close()

    for root, _, files in os.walk(INPUT_DIR):
        for file in files:
            if file.endswith(".mtx"):
                caminho_entrada = os.path.join(root, file)
                nome_base = os.path.splitext(file)[0]
                caminho_saida = os.path.join(OUTPUT_DIR, f"{nome_base}_clean.mtx")

                try:
                    matriz = mmread(caminho_entrada)

                    if issparse(matriz):
                        matriz = matriz.tocsr()
                    else:
                        matriz = csr_matrix(matriz)

                    if matriz.shape[0] != matriz.shape[1]:
                        log(f"ERRO: matriz não quadrada → {file}")
                        continue
                    
                    G = nx.from_scipy_sparse_array(matriz)

                    if isinstance(G, nx.MultiGraph):
                        G = nx.Graph(G)

                    G.remove_edges_from(nx.selfloop_edges(G))

                    mapping = {
                        old_label: new_label
                        for new_label, old_label in enumerate(sorted(G.nodes()))
                    }
                    G = nx.relabel_nodes(G, mapping)

                    matriz_limpa = nx.to_scipy_sparse_array(G, format="csr", dtype=int)
                    mmwrite(caminho_saida, matriz_limpa)

                except Exception as e:
                    log(f"ERRO ao processar {file}: {e}")

if __name__ == "__main__":
    clean_graphs()