#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "cobertura.h"

enum { MAX_VERTICES = 15 };

/* Ordenadas pelo maior extremo. Para cada n, usamos o subgrafo induzido
   pelos vertices de 0 a n-1, com as mesmas arestas para os dois metodos. */
static const Aresta base[] = {
    {0, 1}, {0, 2}, {1, 3}, {2, 4},
    {1, 5}, {4, 5}, {0, 6}, {3, 6}, {5, 6},
    {2, 7}, {5, 7}, {6, 7}, {1, 8}, {4, 8}, {6, 8},
    {2, 9}, {3, 9}, {7, 9}, {8, 9},
    {0, 10}, {5, 10}, {7, 10}, {2, 11}, {6, 11}, {8, 11}, {10, 11},
    {1, 12}, {4, 12}, {9, 12}, {11, 12},
    {3, 13}, {7, 13}, {10, 13}, {12, 13},
    {4, 14}, {8, 14}, {11, 14}, {13, 14}
};

static Grafo grafo_do_experimento(int n) {
    const int total = sizeof base / sizeof base[0];
    Grafo g = {n, 0, base};
    while (g.m < total && base[g.m].v < n) g.m++;
    return g;
}

static void imprimir_conjunto(const char *nome, const int selecionado[], int n) {
    const char *separador = "";
    printf("%s = {", nome);
    for (int v = 0; v < n; v++) {
        if (selecionado[v]) {
            printf("%s%d", separador, v);
            separador = ", ";
        }
    }
    puts("}");
}

typedef int (*Algoritmo)(const Grafo *, int []);

/* Repetir em lotes evita arredondar chamadas curtas para zero no relogio. */
static int medir(Algoritmo algoritmo, const Grafo *g, int saida[], double *media_us) {
    enum { LOTE = 16 };
    clock_t inicio = clock();
    if (inicio == (clock_t)-1) return -1;
    size_t repeticoes = 0;
    double segundos;
    int tamanho = 0;
    do {
        for (int i = 0; i < LOTE; i++) {
            tamanho = algoritmo(g, saida);
            if (tamanho < 0) return -1;
        }
        repeticoes += LOTE;
        clock_t fim = clock();
        if (fim == (clock_t)-1 || fim < inicio) return -1;
        segundos = ((double)fim - (double)inicio) / CLOCKS_PER_SEC;
    } while (segundos < 0.05);
    *media_us = segundos * 1000000.0 / (double)repeticoes;
    return tamanho;
}

int main(void) {
    const int tamanhos[] = {5, 8, 10, 12, 15};
    int exata[MAX_VERTICES], gulosa[MAX_VERTICES];
    for (size_t i = 0; i < sizeof tamanhos / sizeof tamanhos[0]; i++) {
        Grafo g = grafo_do_experimento(tamanhos[i]);
        double tempo_bt, tempo_gulosa;
        int minimo = medir(cobertura_minima_bt, &g, exata, &tempo_bt);
        int aproximado = medir(cobertura_gulosa, &g, gulosa, &tempo_gulosa);
        if (minimo < 0 || aproximado < minimo ||
            !eh_cobertura(&g, exata) || !eh_cobertura(&g, gulosa)) {
            fprintf(stderr, "Falha no experimento de %d vertices.\n", g.n);
            return EXIT_FAILURE;
        }
        if (i == 0) {
            puts("Caminho de exemplo: 3 -- 1 -- 0 -- 2 -- 4");
            imprimir_conjunto("Cobertura minima", exata, g.n);
            imprimir_conjunto("Cobertura gulosa", gulosa, g.n);
            puts("\nExperimento: tamanho da cobertura e tempo medio por chamada (microssegundos)");
            printf("%3s %4s %7s %7s %12s %12s\n",
                   "n", "m", "Minima", "Gulosa", "BT (us)", "Gulosa (us)");
        }
        printf("%3d %4d %7d %7d %12.3f %12.3f\n",
               g.n, g.m, minimo, aproximado, tempo_bt, tempo_gulosa);
    }
    return EXIT_SUCCESS;
}
