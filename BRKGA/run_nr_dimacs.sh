#!/usr/bin/env bash
# Executa o BRKGA para todos os grafos em data/NR_DIMACS_cleaned/
# Apaga o .mtx se a execução for bem-sucedida.
# Logs em data/NR_DIMACS_cleaned/run.log

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DATA_DIR="$SCRIPT_DIR/data/NR_DIMACS_cleaned"
BIN="$SCRIPT_DIR/bin/main"
LOG="$DATA_DIR/run.log"
PARAMS="--p 121 --pe 0.153 --pm 0.2638 --rhoe 0.8288 --X_INTVL 155 --X_NUMBER 1 --MAX_GENS 1650 --MAX_STAGT 85"

# Conta grafos restantes
TOTAL=$(ls "$DATA_DIR"/*.mtx 2>/dev/null | wc -l | tr -d ' ')
if [ "$TOTAL" -eq 0 ]; then
    echo "[$(date '+%F %T')] Nenhum .mtx encontrado em $DATA_DIR. Nada a fazer." | tee -a "$LOG"
    exit 0
fi

echo "[$(date '+%F %T')] === Início do lote ($TOTAL grafos) ===" | tee -a "$LOG"

DONE=0
FAILED=0

for MTX in "$DATA_DIR"/*.mtx; do
    NAME="$(basename "$MTX")"
    echo "[$(date '+%F %T')] [$((DONE+FAILED+1))/$TOTAL] Iniciando: $NAME" | tee -a "$LOG"

    # Executa; captura stdout+stderr no log e na variável de saída
    if OUTPUT=$(cd "$SCRIPT_DIR" && "$BIN" "$MTX" $PARAMS 2>&1); then
        echo "$OUTPUT" >> "$LOG"
        echo "[$(date '+%F %T')] ✓ Sucesso: $NAME — apagando arquivo" | tee -a "$LOG"
        rm -f "$MTX"
        DONE=$((DONE + 1))
    else
        EXIT_CODE=$?
        echo "$OUTPUT" >> "$LOG"
        echo "[$(date '+%F %T')] ✗ Falha (exit $EXIT_CODE): $NAME — arquivo mantido" | tee -a "$LOG"
        FAILED=$((FAILED + 1))
    fi
done

echo "[$(date '+%F %T')] === Fim do lote — sucesso: $DONE | falhas: $FAILED ===" | tee -a "$LOG"
