#!/bin/bash

set -e

NUM_EXECUCOES=5
THREADS_LIST="1 2 4 8"

RAW_CSV="tempos_brutos.csv"
AVG_CSV="tempos_medios.csv"
SPEEDUP_CSV="speedup_eficiencia.csv"

echo "programa,parametros,threads,execucao,tempo_segundos" > "$RAW_CSV"
echo "programa,parametros,threads,tempo_medio_segundos" > "$AVG_CSV"
echo "programa,parametros,threads,tempo_medio_segundos,speedup,eficiencia" > "$SPEEDUP_CSV"

declare -A MEDIA

medir() {
  local programa="$1"
  local parametros="$2"
  local threads="$3"
  local comando="$4"
  local soma="0"
  local i inicio fim tempo media_local

  echo "=============================================="
  echo "MEDINDO: $programa (parametros=$parametros threads=$threads)"
  echo "COMANDO: $comando"
  echo "=============================================="

  for i in $(seq 1 $NUM_EXECUCOES); do
    echo "--- Execução $i ---"
    inicio=$(date +%s.%N)
    eval "$comando"
    fim=$(date +%s.%N)
    tempo=$(awk -v a="$inicio" -v b="$fim" 'BEGIN{printf "%.6f", b-a}')
    echo "Tempo medido (wall-clock): ${tempo}s"
    echo "$programa,$parametros,$threads,$i,$tempo" >> "$RAW_CSV"
    soma=$(awk -v s="$soma" -v t="$tempo" 'BEGIN{printf "%.6f", s+t}')
  done

  media_local=$(awk -v s="$soma" -v n="$NUM_EXECUCOES" 'BEGIN{printf "%.6f", s/n}')
  echo "Tempo médio: ${media_local}s"
  echo ""
  echo "$programa,$parametros,$threads,$media_local" >> "$AVG_CSV"
  MEDIA["${programa}_${parametros}_${threads}"]="$media_local"
}

calcular_speedup() {
  local programa="$1"
  local parametros="$2"
  local t1="${MEDIA["${programa}_${parametros}_1"]}"
  local t threads_atual tp speedup eficiencia

  for t in $THREADS_LIST; do
    tp="${MEDIA["${programa}_${parametros}_${t}"]}"
    speedup=$(awk -v t1="$t1" -v tp="$tp" 'BEGIN{printf "%.4f", t1/tp}')
    eficiencia=$(awk -v sp="$speedup" -v p="$t" 'BEGIN{printf "%.4f", sp/p}')
    echo "$programa,$parametros,$t,$tp,$speedup,$eficiencia" >> "$SPEEDUP_CSV"
  done
}

echo "COMPILANDO: gcc -O2 -Wall contacorrente_seq.c -o contacorrente_seq"
gcc -O2 -Wall contacorrente_seq.c -o contacorrente_seq
echo "COMPILANDO: gcc -O2 -Wall contacorrente_f1.c -o contacorrente_f1 -pthread"
gcc -O2 -Wall contacorrente_f1.c -o contacorrente_f1 -pthread
echo "COMPILANDO: gcc -O2 -Wall contacorrente_f2.c -o contacorrente_f2 -pthread"
gcc -O2 -Wall contacorrente_f2.c -o contacorrente_f2 -pthread
echo "COMPILANDO: gcc -O2 -Wall primos.c -o primos -pthread"
gcc -O2 -Wall primos.c -o primos -pthread
echo "COMPILANDO: gcc -O2 -Wall soma_vetores.c -o soma_vetores -pthread"
gcc -O2 -Wall soma_vetores.c -o soma_vetores -pthread
echo "COMPILANDO: gcc -O2 -Wall matriz_vetor.c -o matriz_vetor -pthread"
gcc -O2 -Wall matriz_vetor.c -o matriz_vetor -pthread
echo ""

medir "contacorrente_seq" "N/A" "1" "./contacorrente_seq"
medir "contacorrente_f1" "N/A" "2" "./contacorrente_f1"
medir "contacorrente_f2" "N/A" "2" "./contacorrente_f2"

for t in $THREADS_LIST; do
  medir "primos" "5000000" "$t" "./primos 5000000 $t"
done
calcular_speedup "primos" "5000000"

for t in $THREADS_LIST; do
  medir "soma_vetores" "50000000" "$t" "./soma_vetores 50000000 $t"
done
calcular_speedup "soma_vetores" "50000000"

for t in $THREADS_LIST; do
  medir "matriz_vetor" "10000" "$t" "./matriz_vetor 10000 $t"
done
calcular_speedup "matriz_vetor" "10000"

medir "seq.py" "N/A" "1" "python3 seq.py"
medir "threads.py" "N/A" "2" "python3 threads.py"

echo "=============================================="
echo "ARQUIVOS GERADOS:"
echo "  $RAW_CSV"
echo "  $AVG_CSV"
echo "  $SPEEDUP_CSV"
echo "=============================================="
