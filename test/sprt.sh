#!/usr/bin/env bash
set -euo pipefail

fastchess \
    -engine cmd=./cubeeb-new name="Cubeeb new" \
    -engine cmd=./cubeeb-old name="Cubeeb old" \
    -each tc=10+0.1 \
    -rounds 100000 \
    -concurrency 10 \
    -sprt elo0=0 elo1=10 alpha=0.05 beta=0.05 \
    -pgnout file=sprt.pgn \
    -repeat \
    #-log file=log.log engine=true
