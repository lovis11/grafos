#include <stdio.h>
#include <stdlib.h>
#include "bellman_ford.h"

enum { N = 6 };

static int mostrar_resultado(GrafoListaPonderado *g, int origem) {
    int dist[N], pred[N];
    printf("Origem %d\n", origem);
    if (!bellman_ford(g, origem, dist, pred))
        return reconstruir_ciclo_negativo(g, pred);

    puts("Vertice  Distancia  Predecessor");
    for (int v = 0; v < g->n; v++) {
        if (v != origem && pred[v] == -1)
            printf("%7d  sem caminho\n", v);
        else
            printf("%7d  %9d  %11d\n", v, dist[v], pred[v]);
    }
    return 1;
}

static int executar_exemplos(GrafoListaPonderado *g) {
    const int arestas[][3] = {
        {0, 1, 4}, {0, 2, 7}, {1, 2, -2},
        {1, 3, 5}, {2, 3, 3}, {3, 4, 2}
    };
    for (size_t i = 0; i < sizeof arestas / sizeof arestas[0]; i++)
        if (!inserir_aresta_direcionada(g, arestas[i][0], arestas[i][1], arestas[i][2]))
            return 0;

    puts("Grafo com aresta negativa e sem ciclo");
    if (!mostrar_resultado(g, 0)) return 0;

    puts("\nInserindo a aresta 3 -> 1 de peso -4");
    if (!inserir_aresta_direcionada(g, 3, 1, -4)) return 0;
    if (!mostrar_resultado(g, 0)) return 0;

    puts("\nBusca a partir do vertice isolado");
    return mostrar_resultado(g, 5);
}

int main(void) {
    GrafoListaPonderado *g = criar_grafo_ponderado(N);
    if (!g) return EXIT_FAILURE;
    int sucesso = executar_exemplos(g);
    liberar_grafo(g);
    if (!sucesso) fputs("Falha ao executar o exemplo.\n", stderr);
    return sucesso ? EXIT_SUCCESS : EXIT_FAILURE;
}
