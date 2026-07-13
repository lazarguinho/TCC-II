#!/usr/bin/env bash
# Executa o BOGA para todos os grafos em data/random/
# Logs em data/random/run.log

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DATA_DIR="$SCRIPT_DIR/data/random"
BIN="$SCRIPT_DIR/bin/main"
LOG="$DATA_DIR/run.log"
PARAMS="--opx cx --opm em --pop 30 --gens 53 --mut 0.446 --cr 0.5724 --max-stagt 1031 --runs 1"

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
