import csv
import os

LIMPAS  = "instancias_HB_Limpas.csv"
TODAS   = "instancias_HB_Todas.csv"
SAIDA   = "instancias_HB_nao_limpas.csv"

script_dir = os.path.dirname(os.path.abspath(__file__))

def ler_nomes(path):
    with open(path, newline="", encoding="utf-8") as f:
        return [linha.strip() for linha in f if linha.strip()]

# Lê limpas e normaliza: remove sufixo "_clean.mtx" para obter o nome base
limpas_raw = ler_nomes(os.path.join(script_dir, LIMPAS))
limpas = {nome.removesuffix("_clean.mtx") for nome in limpas_raw}

# Lê todas (já estão como nome base, sem extensão)
todas = ler_nomes(os.path.join(script_dir, TODAS))

# Instâncias em "todas" que não estão em "limpas"
nao_limpas = [nome for nome in todas if nome not in limpas]

# Escreve resultado
saida_path = os.path.join(script_dir, SAIDA)
with open(saida_path, "w", newline="", encoding="utf-8") as f:
    writer = csv.writer(f)
    for nome in sorted(nao_limpas):
        writer.writerow([nome])

print(f"Total em 'todas':     {len(todas)}")
print(f"Total em 'limpas':    {len(limpas)}")
print(f"Não limpas:           {len(nao_limpas)}")
print(f"Resultado salvo em:   {saida_path}")
