#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "hamiltoniano.h"
#include "tps.h"

enum { MAX_EXPERIMENTO = 12 };
typedef enum { CAMINHO, CICLO, FORCA_BRUTA, VIZINHO, INSERCAO } Metodo;

/* Para cada n, usamos as arestas da base cujos dois extremos sao menores
   que n. Sao os mesmos grafos da pratica 09 nesses tamanhos. */
static GrafoMatriz *grafo_do_experimento(int n) {
    static const int arestas[][2] = {
        {0, 1}, {0, 2}, {1, 3}, {2, 4},
        {1, 5}, {4, 5}, {0, 6}, {3, 6}, {5, 6},
        {2, 7}, {5, 7}, {6, 7}, {1, 8}, {4, 8}, {6, 8},
        {2, 9}, {3, 9}, {7, 9}, {8, 9},
        {0, 10}, {5, 10}, {7, 10}, {2, 11}, {6, 11}, {8, 11}, {10, 11}
    };
    GrafoMatriz *g = criar_grafo_matriz(n);
    if (!g) return NULL;
    for (size_t i = 0; i < sizeof arestas / sizeof arestas[0]; i++) {
        int u = arestas[i][0], v = arestas[i][1];
        if (u < n && v < n) inserir_aresta_matriz(g, u, v);
    }
    return g;
}

static long long executar(Metodo metodo, const GrafoMatriz *matriz, int rota[]) {
    switch (metodo) {
        case CAMINHO:     return encontrar_caminho_hamiltoniano_bt(matriz, rota);
        case CICLO:       return encontrar_ciclo_hamiltoniano_bt(matriz, rota);
        case FORCA_BRUTA: return tsp_forca_bruta(matriz, 0, rota);
        case VIZINHO:     return tsp_vizinho_mais_proximo(matriz, 0, rota);
        case INSERCAO:    return tsp_insercao(matriz, 0, rota);
    }
    return -1;
}

/* As chamadas curtas sao repetidas em lotes para superar a resolucao de
   clock(). Uma unica chamada ja basta quando leva ao menos 25 ms. */
static long long medir(Metodo metodo, const GrafoMatriz *matriz, int rota[], double *media_us) {
    clock_t inicio = clock();
    if (inicio == (clock_t)-1) return -1;
    int lote = 1;
    size_t repeticoes = 0;
    long long resultado = 0;
    for (;;) {
        for (int i = 0; i < lote; i++) {
            resultado = executar(metodo, matriz, rota);
            if (resultado < 0) return -1;
        }
        repeticoes += (size_t)lote;
        clock_t fim = clock();
        if (fim == (clock_t)-1 || fim < inicio) return -1;
        double segundos = ((double)fim - (double)inicio) / CLOCKS_PER_SEC;
        if (segundos >= 0.025) {
            *media_us = segundos * 1000000.0 / (double)repeticoes;
            return resultado;
        }
        if (lote < 256) lote *= 2;
    }
}

static void imprimir_rota(const int rota[], int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        if (i > 0) printf(" -> ");
        printf("%d", rota[i]);
    }
    putchar('\n');
}

static int experimento(int n) {
    static const char *const nomes[] = {
        "Caminho hamiltoniano", "Ciclo hamiltoniano", "TSP forca bruta",
        "TSP vizinho mais proximo", "TSP insercao"
    };
    GrafoMatriz *g = grafo_do_experimento(n);
    if (!g) return 0;
    GrafoMatriz *custos = grafo_para_tsp(g);
    if (!custos) {
        liberar_grafo_matriz(g);
        return 0;
    }

    int rota[MAX_EXPERIMENTO + 1];
    int sucesso = 1;
    printf("\nn = %d\n", n);
    printf("%-24s %9s %14s   %s\n", "Metodo", "Resultado", "Tempo (us)", "Rota");
    for (Metodo m = CAMINHO; m <= INSERCAO; m++) {
        int eh_tsp = m >= FORCA_BRUTA;
        double tempo;
        long long resultado = medir(m, eh_tsp ? custos : g, rota, &tempo);
        if (resultado < 0) {
            sucesso = 0;
            break;
        }
        printf("%-24s ", nomes[m]);
        if (eh_tsp) printf("%9lld ", resultado);
        else printf("%9s ", resultado ? "sim" : "nao");
        printf("%14.3f   ", tempo);
        if (eh_tsp || resultado) imprimir_rota(rota, n + (m != CAMINHO));
        else puts("-");
    }
    liberar_grafo_matriz(custos);
    liberar_grafo_matriz(g);
    return sucesso;
}

int main(void) {
    const int tamanhos[] = {5, 8, 10, MAX_EXPERIMENTO};
    puts("O TSP parte do vertice 0. O resultado de cada metodo TSP e o custo total da volta.");
    puts("Os tempos sao medias por chamada em microssegundos (us).");
    for (size_t i = 0; i < sizeof tamanhos / sizeof tamanhos[0]; i++) {
        if (!experimento(tamanhos[i])) {
            fprintf(stderr, "Falha no experimento de %d vertices.\n", tamanhos[i]);
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}
