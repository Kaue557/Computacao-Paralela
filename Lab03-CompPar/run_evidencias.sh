#!/bin/bash

set -e

echo "=============================================="
echo "PARTE 1 - FASE 0: contacorrente_seq.c"
echo "=============================================="
echo "COMPILANDO: gcc -O2 -Wall contacorrente_seq.c -o contacorrente_seq"
gcc -O2 -Wall contacorrente_seq.c -o contacorrente_seq
echo "EXECUTANDO: ./contacorrente_seq"
./contacorrente_seq

echo ""
echo "=============================================="
echo "PARTE 1 - FASE 1: contacorrente_f1.c"
echo "=============================================="
echo "COMPILANDO: gcc -O2 -Wall contacorrente_f1.c -o contacorrente_f1 -pthread"
gcc -O2 -Wall contacorrente_f1.c -o contacorrente_f1 -pthread
echo "EXECUTANDO 5 VEZES: ./contacorrente_f1"
for i in 1 2 3 4 5; do
  echo "--- Execução $i ---"
  ./contacorrente_f1
done

echo ""
echo "=============================================="
echo "PARTE 1 - FASE 2: contacorrente_f2.c"
echo "=============================================="
echo "COMPILANDO: gcc -O2 -Wall contacorrente_f2.c -o contacorrente_f2 -pthread"
gcc -O2 -Wall contacorrente_f2.c -o contacorrente_f2 -pthread
echo "EXECUTANDO 5 VEZES: ./contacorrente_f2"
for i in 1 2 3 4 5; do
  echo "--- Execução $i ---"
  ./contacorrente_f2
done

echo ""
echo "=============================================="
echo "PARTE 2 - OPÇÃO A: primos.c (K=5000000)"
echo "=============================================="
echo "COMPILANDO: gcc -O2 -Wall primos.c -o primos -pthread"
gcc -O2 -Wall primos.c -o primos -pthread
for t in 1 2 4 8; do
  echo "EXECUTANDO: ./primos 5000000 $t"
  ./primos 5000000 $t
done

echo ""
echo "=============================================="
echo "PARTE 2 - OPÇÃO C: soma_vetores.c (N=50000000)"
echo "=============================================="
echo "COMPILANDO: gcc -O2 -Wall soma_vetores.c -o soma_vetores -pthread"
gcc -O2 -Wall soma_vetores.c -o soma_vetores -pthread
for t in 1 2 4 8; do
  echo "EXECUTANDO: ./soma_vetores 50000000 $t"
  ./soma_vetores 50000000 $t
done

echo ""
echo "=============================================="
echo "PARTE 2 - OPÇÃO E: matriz_vetor.c (N=10000)"
echo "=============================================="
echo "COMPILANDO: gcc -O2 -Wall matriz_vetor.c -o matriz_vetor -pthread"
gcc -O2 -Wall matriz_vetor.c -o matriz_vetor -pthread
for t in 1 2 4 8; do
  echo "EXECUTANDO: ./matriz_vetor 10000 $t"
  ./matriz_vetor 10000 $t
done

echo ""
echo "=============================================="
echo "PARTE 3: seq.py"
echo "=============================================="
echo "EXECUTANDO: python3 seq.py"
python3 seq.py

echo ""
echo "=============================================="
echo "PARTE 3: threads.py"
echo "=============================================="
echo "EXECUTANDO: python3 threads.py"
python3 threads.py

echo ""
echo "=============================================="
echo "FIM DA EXECUÇÃO"
echo "=============================================="