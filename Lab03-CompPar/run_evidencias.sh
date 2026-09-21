#!/usr/bin/env bash

###############################################################################
# run_evidencias.sh
#
# Script para gerar as evidencias experimentais (screenshots) exigidas na
# Secao 7.2, item 3 do relatorio do Laboratorio de Pthreads.
#
# Uso:
#   chmod +x run_evidencias.sh
#   ./run_evidencias.sh
#
# Dica para as capturas de tela: rode cada bloco separadamente (comente os
# demais, ou rode o script inteiro e va tirando prints conforme os cabecalhos
# aparecem no terminal). Os separadores "====" facilitam recortar a imagem.
###############################################################################

# ----- Parametros ajustaveis --------------------------------------------------
K_PRIMOS=5000000          # intervalo [1, K] para busca de primos
N_VETORES=50000000        # tamanho dos vetores na soma de vetores
N_MATRIZ=10000            # dimensao N da matriz NxN e do vetor V
THREADS_LIST="1 2 4 8"    # quantidades de threads a testar na Parte 2

CFLAGS="-O2 -Wall -pthread"

separador () {
    echo ""
    echo "==============================================================="
    echo "  $1"
    echo "==============================================================="
}

###############################################################################
# PARTE 1 - PROBLEMA DA CONTA CORRENTE
###############################################################################

separador "PARTE 1 - COMPILACAO"

echo "\$ gcc $CFLAGS contacorrente_seq.c -o contacorrente_seq -pthread"
gcc -O2 -Wall contacorrente_seq.c -o contacorrente_seq -pthread

echo "\$ gcc $CFLAGS contacorrente_f1.c -o contacorrente_f1 -pthread"
gcc $CFLAGS contacorrente_f1.c -o contacorrente_f1 -pthread

echo "\$ gcc $CFLAGS contacorrente_f2.c -o contacorrente_f2 -pthread"
gcc $CFLAGS contacorrente_f2.c -o contacorrente_f2 -pthread

separador "PARTE 1 - FASE 0: BASELINE SEQUENCIAL"
./contacorrente_seq

separador "PARTE 1 - FASE 1: CONDICAO DE CORRIDA (5 EXECUCOES)"
echo "Observe que o saldo final varia (e diverge de 150001000.00) a cada rodada:"
for i in 1 2 3 4 5; do
    echo "--- Execucao $i ---"
    ./contacorrente_f1
done

separador "PARTE 1 - FASE 2: CORRECAO COM MUTEX (5 EXECUCOES)"
echo "Observe que o saldo final e sempre 150001000.00 (deterministico):"
for i in 1 2 3 4 5; do
    echo "--- Execucao $i ---"
    ./contacorrente_f2
done

###############################################################################
# PARTE 2 - MENU DE PROBLEMAS (3 opcoes escolhidas)
###############################################################################

separador "PARTE 2 - COMPILACAO"

echo "\$ gcc $CFLAGS primos.c -o primos -pthread"
gcc $CFLAGS primos.c -o primos -pthread

echo "\$ gcc $CFLAGS soma_vetores.c -o soma_vetores -pthread"
gcc $CFLAGS soma_vetores.c -o soma_vetores -pthread

echo "\$ gcc $CFLAGS matriz_vetor.c -o matriz_vetor -pthread"
gcc $CFLAGS matriz_vetor.c -o matriz_vetor -pthread

# --- Opcao A: Primos ---------------------------------------------------------
separador "PARTE 2 - OPCAO A: PRIMOS (K=$K_PRIMOS)"
for t in $THREADS_LIST; do
    echo "--- ./primos $K_PRIMOS $t ---"
    ./primos $K_PRIMOS $t
done

# --- Opcao C: Soma de Vetores -------------------------------------------------
separador "PARTE 2 - OPCAO C: SOMA DE VETORES (N=$N_VETORES)"
for t in $THREADS_LIST; do
    echo "--- ./soma_vetores $N_VETORES $t ---"
    ./soma_vetores $N_VETORES $t
done

# --- Opcao E: Matriz x Vetor --------------------------------------------------
separador "PARTE 2 - OPCAO E: MATRIZ x VETOR (N=$N_MATRIZ)"
for t in $THREADS_LIST; do
    echo "--- ./matriz_vetor $N_MATRIZ $t ---"
    ./matriz_vetor $N_MATRIZ $t
done

###############################################################################
# PARTE 3 - DESAFIO DO PYTHON E O GIL
###############################################################################

separador "PARTE 3 - PYTHON SEQUENCIAL (seq.py)"
echo "\$ python3 seq.py"
python3 seq.py

separador "PARTE 3 - PYTHON COM THREADS (threads.py)"
echo "\$ python3 threads.py"
python3 threads.py

separador "FIM DA EXECUCAO"
echo "Todas as evidencias foram geradas. Revise a saida acima e tire os prints"
echo "correspondentes a cada secao para o relatorio."
