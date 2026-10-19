// Kauê Lima Rodrigues Meneses - 10410594
/*
 Exercício 1 - Regra do Trapézio com OpenMP (slides ch5, págs. 24 a 38)
 
  Compilar: gcc -fopenmp -O2 trapezio_omp.c -o trapezio_omp
  Executar: ./trapezio_omp <threads> <versao>      (depois digite: a b n)
    versao 1: pág. 24-25  região paralela + Trap() com critical no fim
    versao 2: pág. 30-31  critical em volta da chamada (força execução sequencial)
    versao 3: pág. 32     variável privada + critical depois da chamada
    versao 4: pág. 35     cláusula reduction na região paralela
    versao 5: pág. 38     parallel for + reduction (versão final)
 Exemplo: echo "0 3 1000000" | ./trapezio_omp 4 5     (integral de x^2 em [0,3] = 9)
 */
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

double f(double x) { return x * x; }

/* Cada thread calcula a integral do seu pedaço [local_a, local_b] */
double Local_trap(double a, double b, int n) {
    int    my_rank      = omp_get_thread_num();
    int    thread_count = omp_get_num_threads();
    double h        = (b - a) / n;
    int    local_n  = n / thread_count;            /* trapézios por thread */
    double local_a  = a + my_rank * local_n * h;   /* início do meu pedaço */
    double local_b  = local_a + local_n * h;       /* fim do meu pedaço   */
    double my_result = (f(local_a) + f(local_b)) / 2.0;
    for (int i = 1; i <= local_n - 1; i++)
        my_result += f(local_a + i * h);
    return my_result * h;
}

/* Versão 1 (págs. 24-25): Trap faz o cálculo e soma no resultado global */
void Trap(double a, double b, int n, double *global_result_p) {
    double my_result = Local_trap(a, b, n);
#   pragma omp critical
    *global_result_p += my_result;
}

int main(int argc, char *argv[]) {
    if (argc < 2) { fprintf(stderr, "Uso: %s <threads> [versao 1-5]\n", argv[0]); return 1; }
    int    thread_count = strtol(argv[1], NULL, 10);
    int    versao = (argc > 2) ? strtol(argv[2], NULL, 10) : 5;
    double global_result = 0.0, a, b;
    int    n;

    printf("Enter a, b, and n\n");
    scanf("%lf %lf %d", &a, &b, &n);
    if (n % thread_count != 0 && versao <= 4) {
        fprintf(stderr, "Nas versoes 1-4, n deve ser multiplo do numero de threads\n");
        return 1;
    }

    double t0 = omp_get_wtime();
    switch (versao) {
    case 1:
#       pragma omp parallel num_threads(thread_count)
        Trap(a, b, n, &global_result);
        break;
    case 2:
#       pragma omp parallel num_threads(thread_count)
#       pragma omp critical                      /* critical engloba TODO o cálculo */
        global_result += Local_trap(a, b, n);
        break;
    case 3:
#       pragma omp parallel num_threads(thread_count)
        {
            double my_result = Local_trap(a, b, n);   /* cálculo em paralelo */
#           pragma omp critical
            global_result += my_result;               /* só a soma é serial  */
        }
        break;
    case 4:
#       pragma omp parallel num_threads(thread_count) reduction(+: global_result)
        global_result += Local_trap(a, b, n);
        break;
    case 5: {
        double h = (b - a) / n;
        double approx = (f(a) + f(b)) / 2.0;
        int i;
#       pragma omp parallel for num_threads(thread_count) reduction(+: approx)
        for (i = 1; i <= n - 1; i++)
            approx += f(a + i * h);
        global_result = h * approx;
        break;
    }
    default:
        fprintf(stderr, "Versao invalida\n"); return 1;
    }
    double t1 = omp_get_wtime();

    printf("Versao %d, %d threads, n = %d trapezoids, our estimate\n", versao, thread_count, n);
    printf("of the integral from %f to %f = %.14e\n", a, b, global_result);
    printf("Tempo: %.4f s\n", t1 - t0);
    return 0;
}