#!/usr/bin/env bash
set -euo pipefail

fastchess \
    -engine cmd=./cubeeb name="Cubeeb new" \
    -engine cmd=./cubeeb name="Cubeeb old" \
    -each tc=10+0.1 \
    -rounds 100000 \
    -concurrency 10 \
    -sprt elo0=0 elo1=10 alpha=0.05 beta=0.05 \
    -pgnout file=results/sprt.pgn \
    -repeat
