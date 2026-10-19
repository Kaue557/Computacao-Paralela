#!/bin/bash
# Roda as 2 versões com 1..6 threads, 10 execuções cada, e grava resultados.csv
# Escalabilidade forte: N fixo.  Escalabilidade fraca: N = threads * BASE (máx = 2^30).
gcc -fopenmp -O2 soma_omp.c -o soma_omp || exit 1

N_FORTE=268435456          # 2^28 (mínimo pedido). Pode subir até 2^30 se couber na RAM.
N_MAX=1073741824           # 2^30
BASE=$((N_MAX / 6))        # elementos por thread na escalabilidade fraca
REPS=10

echo "tipo,versao,threads,N,tempo_medio_s" > resultados.csv
for versao in 1 2; do
  for t in 1 2 3 4 5 6; do
    echo ">> forte  versao=$versao threads=$t"
    ./soma_omp $N_FORTE $t $versao $REPS | tee /dev/stderr | grep '^CSV' | sed "s/^CSV,/forte,/" >> resultados.csv
    echo ">> fraca  versao=$versao threads=$t"
    ./soma_omp $((BASE * t)) $t $versao $REPS | tee /dev/stderr | grep '^CSV' | sed "s/^CSV,/fraca,/" >> resultados.csv
  done
done
echo "Pronto: resultados.csv"
