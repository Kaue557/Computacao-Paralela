#!/bin/bash
# Roda a versão sequencial e a OpenMP (1..6 threads), 3 execuções cada.
# Gera resultados_ex3.csv com separador ';' e decimal com vírgula (abre direto no Calc/Excel pt-BR).
# Uso: ./rodar_testes_ex3.sh [N]      (padrão N=2048: 3 matrizes, ~100 MB de RAM)
N=${1:-2048}
REPS=3

gcc -O2 matmul_seq.c -o matmul_seq -lm || exit 1
gcc -fopenmp -O2 matmul_omp.c -o matmul_omp -lm || exit 1

# converte a linha "CSV,versao,threads,N,tempo" em "versao;threads;N;tempo" (ponto -> vírgula)
converte() { awk -F, '/^CSV/ { gsub(/\./, ",", $5); print $2 ";" $3 ";" $4 ";" $5 }'; }

echo "versao;threads;N;tempo_medio_s" > resultados_ex3.csv

echo ">> sequencial (N=$N)"
./matmul_seq $N $REPS | tee /dev/stderr | converte >> resultados_ex3.csv

for t in 1 2 3 4 5 6; do
  echo ">> OpenMP com $t thread(s) (N=$N)"
  ./matmul_omp $N $t $REPS | tee /dev/stderr | converte >> resultados_ex3.csv
done
echo "Pronto: resultados_ex3.csv"
